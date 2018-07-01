#include <iostream>
#include <memory>
#include <thread>
#include <math.h>
#include <GL/glew.h>
#include <fftw3.h>
#include "spectrum.h"

struct
{
    float fft_size = 4096*2;
    float window_size = 1024;
    float window_overlap = 0.5;
    float y_min = -100.0;
    float y_max = 0.0;
} constants;

const std::string SpectrumVisualizer::name = "spectrum";
const std::string title = "Spectrum Visualizer";

void SpectrumVisualizer::loadConfig(const IniParser& ini)
{
    constants.fft_size = ini.getOptionAsUnsignedInteger("spectrum", "fft_size");
    constants.window_size = ini.getOptionAsUnsignedInteger("spectrum", "window_size");
}

SpectrumVisualizer::SpectrumVisualizer()
    :
    Visualizer(),
    spectrum(std::vector<float>(constants.fft_size/2+1)),
    shader("spectrum"),
    palette{16, {
        {0.0, {1.0, 0.0, 0.0}},
        {0.5, {1.0, 0.0, 1.0}},
        {0.6, {1.0, 1.0, 1.0}},
        {1.0, {1.0, 1.0, 1.0}},
    }}
{}

SpectrumVisualizer::~SpectrumVisualizer()
{}

const std::string& SpectrumVisualizer::getName() const
{
    return this->name;
}

const std::string& SpectrumVisualizer::getTitle() const
{
    return title;
}

void SpectrumVisualizer::audioThreadFunc()
{
    SimpleRecordClient src(10 * 1000, "pulseviz", "spectrum");
    STFT stft(
        src,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFT::Window::HAMMING
    );

    while (!this->quit_thread)
    {
        stft.slide();

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
    glOrtho(0.0, 1.0, constants.y_min, constants.y_max, -1.0, 1.0);
}

void SpectrumVisualizer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

#if 0
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glColor3f(0.1, 0.1, 0.1);
    glVertex2f(0.0, 0.0);
    glVertex2f(1.0, 0.0);
    glEnd();
#endif

    this->data_mutex.lock();

    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 0);
    glBindTexture(GL_TEXTURE_1D, this->palette.getHandle());

    this->shader.bind();
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    float x = 0.0;
    unsigned int i = 0;
    for (float& y: this->spectrum) {
        // TODO: Scaling can be done in the shader
        // x = (float)i / (float)this->spectrum.size();
        //x = 1.0 - (pow(1000.0, 1.0 - ((float)i / (float)this->spectrum.size())) - 1.0) / 999.0;
        // TODO: log10(0) ?!
        x = pow(10, 1.0/2.0 * log10((float)i / (float)this->spectrum.size()));
        float magnitude = (y - constants.y_min) / (constants.y_max - constants.y_min);
        i += 1;
        glColor3f(1.0 - x, 0.0, x);
        glTexCoord2f(magnitude, magnitude);
        glVertex2f(x, y);
    }
    glEnd();
    this->shader.unbind();

    this->data_mutex.unlock();
}

void SpectrumVisualizer::attachSRC()
{

    this->quit_thread = false;
    this->audio_thread = std::thread([this] {
        this->audioThreadFunc();
    });
}

void SpectrumVisualizer::detatchSRC()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

SpectrumVisualizerFactory::SpectrumVisualizerFactory(const IniParser& ini)
{
    SpectrumVisualizer::loadConfig(ini);
}

std::unique_ptr<Visualizer> SpectrumVisualizerFactory::create() const
{
    return std::unique_ptr<Visualizer>(new SpectrumVisualizer());
}
