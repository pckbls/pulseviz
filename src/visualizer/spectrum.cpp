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
        STFT::Window::HAMMING,
        STFT::Weighting::Z
    ),
    spectrum(stft.coefficients.size()),
    shader("spectrum"),
    palette{64, {
        {0.00, {0.0f, 0.0f, 0.0f}},
        {0.20, {0.0f, 0.0f, 0.5f}},
        {0.45, {1.0f, 0.0f, 1.0f}},
        {0.80, {1.0f, 1.0f, 0.0f}},
        {1.00, {1.0f, 1.0f, 1.0f}}
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
        // TODO: Why do we do this?! Couldn't this be removed for improved performance?
        // Or at least use a copy function..
        for (unsigned int i = 0; i < this->spectrum.size(); i++)
            this->spectrum[i] = stft.coefficients[i];
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

float SpectrumVisualizer::frequency2xCoordinate(float frequency) const
{
    // TODO: It is sufficient to calculate those once!
    float b = log(this->stft.getFrequencies().back() / this->stft.getFrequencies()[1]) / (1.0f - 0.0f);
    float a = this->stft.getFrequencies().back() / exp(b * 1.0f);
    return log(frequency / a) / b;
}

void SpectrumVisualizer::drawGrid() const
{
    // TODO: Generate those in a smart matter depending on the chosen sample rate
    std::vector<float> frequencies = {
        10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f,
        100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f, 800.0f, 900.0f,
        1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f,
        10000.0f, 20000.0f, 30000.0f, 40000.0f, 50000.0f, 60000.0f, 70000.0f, 80000.0f, 90000.0f,
    };

    // TODO: Generate those automatically!
    std::vector<float> levels = {
        -100.0f, -90.0f, -80.0f, -70.0f, -60.0f, -50.0f, -40.0f, -30.0f, -20.0f, -10.0f, 0.0f
    };

    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);

    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glColor3f(0.1f, 0.1f, 0.1f);

    // Vertical lines
    for (const auto& frequency: frequencies)
    {
        float x = this->frequency2xCoordinate(frequency);
        glVertex2f(x, this->factory.dB_min);
        glVertex2f(x, this->factory.dB_max);
    }

    // Horizontal lines
    for (const auto& level: levels)
    {
        glVertex2f(0.0f, level);
        glVertex2f(1.0f, level);
    }

    // Horizontal clipping line
    {
        auto level = this->factory.dB_clip;
        glVertex2f(0.0f, level);
        glVertex2f(1.0f, level);
    }

    glEnd();
}

void SpectrumVisualizer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (this->factory.enable_grid)
        this->drawGrid();

    this->data_mutex.lock();

    this->shader.bind();
    this->shader.bindTextureToUniform("palette", this->palette, 0);

    glLineWidth(1.0f);
    glBegin(GL_LINE_STRIP);

    // We start at i = 1 because we do not want to draw the DC component
    // that is stored at i = 0.
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
    this->fill = ini.getOptionAsBool("spectrum", "fill");
    this->enable_grid = ini.getOptionAsBool("spectrum", "grid");

    if (this->fill)
        throw "spectrum.fill has not been implemented yet.";
}

std::unique_ptr<Visualizer> SpectrumVisualizerFactory::create() const
{
    auto* visualizer = new SpectrumVisualizer(*this);
    return std::unique_ptr<Visualizer>(visualizer);
}
