#include <iostream>
#include <memory>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "waveform.h"

// TODO: Use the same rendering trick as in our spectrum analyzer!
// That way we could render huge chunks of data

const std::string WaveFormVisualizer::name = "waveform";
const std::string title = "Waveform Visualizer";

WaveFormVisualizer::Config WaveFormVisualizer::config;

void WaveFormVisualizer::loadConfig(const IniParser& ini)
{
    config.buffer_size = ini.getOptionAsUnsignedInteger("waveform", "buffer_size");
}

WaveFormVisualizer::WaveFormVisualizer()
    :
    Visualizer(),
    shader("waveform"),
    palette{16, {
        {0.0, {1.0, 0.0, 0.0}},
        {0.5, {1.0, 0.0, 1.0}},
        {0.6, {1.0, 1.0, 1.0}},
        {1.0, {1.0, 1.0, 1.0}},
    }}
{}

WaveFormVisualizer::~WaveFormVisualizer()
{}

const std::string& WaveFormVisualizer::getName() const
{
    return this->name;
}

const std::string& WaveFormVisualizer::getTitle() const
{
    return title;
}

void WaveFormVisualizer::audioThreadFunc()
{
    SimpleRecordClient src(10 * 1000, "pulseviz", "waveform");
    Sampler sampler(src, config.buffer_size, 256);

    this->samples.resize(sampler.data.size());

    while (!this->quit_thread)
    {
        sampler.readChunk();
        this->mutex.lock();
        sampler.appendToBuffer();
        this->samples = sampler.data;
        this->mutex.unlock();
    }
}

void WaveFormVisualizer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 0);
    glBindTexture(GL_TEXTURE_1D, this->palette.getHandle());

    this->mutex.lock();
    this->shader.bind();
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    float x = 0.0;
    for (float& y: this->samples) {
        x += 1.0 / this->samples.size();
        glColor3f(1.0f - x, 0.0f, x);
        glTexCoord2f(0.0f, std::abs(y) / 2.0f + 0.3f);
        glVertex2f(x, y);
    }
    glEnd();
    this->shader.unbind();
    this->mutex.unlock();
}

void WaveFormVisualizer::attachSRC()
{

    this->quit_thread = false;
    this->audio_thread = std::thread([this] {
        this->audioThreadFunc();
    });
}

void WaveFormVisualizer::detatchSRC()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

WaveFormVisualizerFactory::WaveFormVisualizerFactory(const IniParser& ini)
{
    WaveFormVisualizer::loadConfig(ini);
}

std::unique_ptr<Visualizer> WaveFormVisualizerFactory::create() const
{
    return std::unique_ptr<Visualizer>(new WaveFormVisualizer());
}
