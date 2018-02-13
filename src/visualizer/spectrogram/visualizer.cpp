#include <iostream>
#include "visualizer.h"

const struct
{
    float fft_size = 2048;
    float window_size = 1024;
    float window_overlap = 0.5;
    float y_min = -70.0;
    float y_max = 0.0;
    bool scrolling = true;
} constants;

SpectrogramVisualizer::SpectrogramVisualizer()
    :
    Visualizer(),
    stft(
        this->src,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFTWindow::HAMMING
    ),
    texture(this->stft, 2048),
    shader("spectrogram")
{
    this->startThread();
}

SpectrogramVisualizer::~SpectrogramVisualizer()
{
    this->stopThread();
}

const char* SpectrogramVisualizer::getTitle()
{
    return "Spectrogram Visualizer";
}

void SpectrogramVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->stft.slide();
        this->render_mutex.lock();
        this->texture.updateTextureData();
        this->render_mutex.unlock();
    }
}

void SpectrogramVisualizer::onFramebuffersizeChanged(unsigned int /* width */, unsigned int /* height */)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1.0, 0.0, 1.0, -1, 1);
}

void SpectrogramVisualizer::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    this->render_mutex.lock();

    this->texture.updateTexture();


    float start_index = 0.0;
    if (constants.scrolling)
    {
        // TODO: Explain the cleverness!
        // TODO: Really +1?
        start_index = (float)this->texture.getCursorPosition() / (float)this->texture.getHistorySize();

        float column_width = 1.0f / (float)this->texture.getHistorySize();
        start_index += column_width;
    }
    float end_index = start_index + (float)(this->texture.getHistorySize() - 1) / (float)this->texture.getHistorySize();

    this->shader.bind();
    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("texture"), 0);
    glBindTexture(GL_TEXTURE_2D, this->texture.getHandle());

    glActiveTexture(GL_TEXTURE1 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 1);
    glBindTexture(GL_TEXTURE_1D, this->colorscheme->getPaletteTexture().getHandle());

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, start_index); glVertex2f(0.0, 0.0);
    glTexCoord2f(0.0, end_index); glVertex2f(1.0, 0.0);
    glTexCoord2f(1.0, end_index); glVertex2f(1.0, 1.0);
    glTexCoord2f(1.0, start_index); glVertex2f(0.0, 1.0);
    glEnd();

    this->shader.unbind();

    this->render_mutex.unlock();
}
