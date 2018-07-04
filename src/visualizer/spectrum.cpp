#include <iostream>
#include <memory>
#include <thread>
#include <math.h>
#include <GL/glew.h>
#include <fftw3.h>
#include "spectrum.h"

SpectrumVisualizer::SpectrumVisualizer(const SpectrumVisualizerFactory &factory)
    :
    Visualizer(),
    factory(factory),
    src(10 * 1000, "pulseviz", "spectrum"),
    stft(
        this->src,
        factory.fft_size,
        1024,
        0.5,
        STFT::Window::HAMMING
    ),
    spectrum(stft.coefficients.size()),
    shader("spectrum"),
    palette{16, {
        {0.0, {0.0, 0.0, 1.0}},
        {0.75, {1.0, 0.0, 1.0}},
        {1.0, {1.0, 1.0, 1.0}}
    }}
{
    this->quit_thread = false;
    this->audio_thread = std::thread([this] {
        this->audioThreadFunc();
    });
}

SpectrumVisualizer::~SpectrumVisualizer()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

const std::string SpectrumVisualizer::getTitle() const
{
    return "Spectrum Visualizer";
}

void SpectrumVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->stft.slide();

        this->data_mutex.lock();
        for (unsigned int i = 0; i < this->spectrum.size(); i++)
            this->spectrum[i] = STFT::convertToDecibel(stft.coefficients[i]);
        this->data_mutex.unlock();
    }
}

void SpectrumVisualizer::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        0.0, 1.0,
        // TODO: Use 0.0, 1.0?
        static_cast<double>(this->factory.dB_min), static_cast<double>(this->factory.dB_max),
        -1.0, 1.0
    );
}

float SpectrumVisualizer::frequency2xCoordinate(float frequency)
{
    // TODO: It is sufficient to calculate those once!
    float b = log(this->stft.getFrequencies().back() / this->stft.getFrequencies()[1]) / (1.0f - 0.0f);
    float a = this->stft.getFrequencies().back() / exp(b * 1.0f);
    return log(frequency / a) / b;
}

void SpectrumVisualizer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    this->data_mutex.lock();

    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 0);
    glBindTexture(GL_TEXTURE_1D, this->palette.getHandle());

    this->shader.bind();
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);

    for (unsigned int i = 1; i < this->spectrum.size(); i++)
    {
        float frequency = this->stft.getFrequencies()[i];
        float x = this->frequency2xCoordinate(frequency);
        float y = this->spectrum[i];
        float normalized_magnitude = (y - this->factory.dB_min) / (this->factory.dB_clip - this->factory.dB_min);

        glTexCoord1f(normalized_magnitude);
        glVertex2f(x, y);
    }

    glEnd();
    this->shader.unbind();

    this->data_mutex.unlock();
}

SpectrumVisualizerFactory::SpectrumVisualizerFactory(const IniParser& ini)
{
    this->dB_min = ini.getOptionAsFloat("general", "dB_min");
    this->dB_max = ini.getOptionAsFloat("general", "dB_max");
    this->dB_clip = ini.getOptionAsFloat("general", "dB_clip");
    this->fft_size = ini.getOptionAsUnsignedInteger("fft", "fft_size");
}

std::unique_ptr<Visualizer> SpectrumVisualizerFactory::create() const
{
    auto* visualizer = new SpectrumVisualizer(*this);
    return std::unique_ptr<Visualizer>(visualizer);
}
