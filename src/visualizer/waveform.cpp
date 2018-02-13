#include <iostream>
#include <memory>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "waveform.h"

// TODO: Use the same rendering trick as in our spectrum analyzer!
// That way we could render huge chunks of data

WaveFormVisualizer::WaveFormVisualizer()
    :
    Visualizer(),
    sampler(this->src, 8192, 512),
    shader("waveform")
{
    this->startThread();
}

WaveFormVisualizer::~WaveFormVisualizer()
{
    this->stopThread(); // TODO: Unfortunately we have to call this there...
}

const char* WaveFormVisualizer::getTitle()
{
    return "Wave Form Visualizer";
}

void WaveFormVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->sampler.readChunk();
        this->mutex.lock();
        this->sampler.appendToBuffer();
        this->mutex.unlock();
    }
}

void WaveFormVisualizer::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 0);
    glBindTexture(GL_TEXTURE_1D, this->colorscheme->getPaletteTexture().getHandle());

    this->mutex.lock();
    this->shader.bind();
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    float x = 0.0;
    for (float& y: this->sampler.data) {
        x += 1.0 / this->sampler.data.size();
        glColor3f(1.0 - x, 0.0, x);
        glTexCoord2f(0.0, std::abs(y) / 2.0 + 0.3);
        glVertex2f(x, y);
    }
    glEnd();
    this->shader.unbind();
    this->mutex.unlock();
}
