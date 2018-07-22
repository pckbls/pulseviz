#include <iostream>
#include <cmath>
#include "spectrogram.h"

SpectrogramVisualizer::SpectrogramVisualizer(const SpectrogramVisualizerFactory& factory)
    :
    Visualizer(),
    factory(factory),
    src(10 * 1000, "pulseviz", "spectrogram"),
    stft(
        this->src,
        factory.fft_size,
        1024,
        0.5,
        STFT::Window::HAMMING,
        STFT::Weighting::Z
    ),
    // TODO: Explain why - 1!
    row(stft.coefficients.size() - 1),
    // TODO: Unfortunately texture size is capped on most GPUs, still make it possible to use large fft_sizes
    // in combination with smaller texture sizes. Could be achieved by drawing new data sets using a FBO.
    texture(factory.history_size, row.size()),
    shader("spectrogram"),
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

SpectrogramVisualizer::~SpectrogramVisualizer()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

const std::string SpectrogramVisualizer::getTitle() const
{
    return "Spectrogram Visualizer";
}

void SpectrogramVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->stft.slide();
        this->render_mutex.lock();
        // TODO: I think this section could be optimized. A for loop looks expensive.
        for (unsigned int i = 1; i < this->stft.coefficients.size(); i++)
            this->row[i] = (this->stft.coefficients[i] - this->factory.dB_min) / (this->factory.dB_clip - this->factory.dB_min);
        this->render_mutex.unlock();
    }
}

void SpectrogramVisualizer::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1.0, 0.0, 1.0, -1, 1);
}

void SpectrogramVisualizer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    this->render_mutex.lock();
    this->texture.insertRow(this->row);
    this->render_mutex.unlock();

    float start_index, end_index;
    if (this->factory.scrolling)
    {
        auto coordinates = this->texture.getCursorCoordinates();
        start_index = coordinates.first;
        end_index = coordinates.second;
    }
    else
    {
        start_index = 0.0f;
        end_index = 1.0f; // TODO: I guss this is wrong, the right edge should be wrong.
    }

    this->shader.bind();
    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("texture"), 0);
    glBindTexture(GL_TEXTURE_2D, this->texture.getHandle());

    glActiveTexture(GL_TEXTURE1 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 1);
    glBindTexture(GL_TEXTURE_1D, this->palette.getHandle());

    float b = log(this->stft.getFrequencies().back() / this->stft.getFrequencies()[1]) / (1.0f - 0.0f);
    glUniform1f(this->shader.getUniformLocation("coord_coeff"), b);
    glUniform1f(this->shader.getUniformLocation("freq_min"), this->stft.getFrequencies()[1]);
    glUniform1f(this->shader.getUniformLocation("freq_max"), this->stft.getFrequencies().back());

    glBegin(GL_QUADS);
    if (!this->factory.vertical)
    {
        glTexCoord2f(0.0, start_index); glVertex2f(0.0, 0.0);
        glTexCoord2f(0.0, end_index); glVertex2f(1.0, 0.0);
        glTexCoord2f(1.0, end_index); glVertex2f(1.0, 1.0);
        glTexCoord2f(1.0, start_index); glVertex2f(0.0, 1.0);
    }
    else
    {
        glTexCoord2f(0.0, end_index); glVertex2f(0.0, 1.0);
        glTexCoord2f(0.0, start_index); glVertex2f(0.0, 0.0);
        glTexCoord2f(1.0, start_index); glVertex2f(1.0, 0.0);
        glTexCoord2f(1.0, end_index); glVertex2f(1.0, 1.0);
    }
    glEnd();

    this->shader.unbind();
}

SpectrogramVisualizerFactory::SpectrogramVisualizerFactory(const IniParser& ini)
{
    this->dB_min = ini.getOptionAsFloat("general", "dB_min");
    this->dB_max = ini.getOptionAsFloat("general", "dB_max");
    this->dB_clip = ini.getOptionAsFloat("general", "dB_clip");
    this->fft_size = ini.getOptionAsUnsignedInteger("fft", "fft_size");

    this->history_size = ini.getOptionAsUnsignedInteger("spectrogram", "history_size");
    this->scrolling = ini.getOptionAsBool("spectrogram", "scrolling");
    this->vertical = ini.getOptionAsBool("spectrogram", "vertical");
}

std::unique_ptr<Visualizer> SpectrogramVisualizerFactory::create() const
{
    auto* visualizer = new SpectrogramVisualizer(*this);
    return std::unique_ptr<Visualizer>(visualizer);
}
