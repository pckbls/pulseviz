#include <iostream>
#include "spectrogram.h"

struct
{
    unsigned int fft_size = 4096;
    unsigned int window_size = 1024;
    unsigned int history_size = 512;
    float window_overlap = 0.5;
    float y_min = -70.0;
    float y_max = 0.0;
    bool scrolling = true;
} constants;

void SpectrogramVisualizer::loadConfig(const IniParser& ini)
{
    constants.scrolling = ini.getOptionAsBool("spectrogram", "scrolling");
    constants.history_size = ini.getOptionAsUnsignedInteger("spectrogram", "history_size");
}

SpectrogramVisualizer::SpectrogramVisualizer()
    :
    Visualizer(),
    row((constants.fft_size / 2) + 1),
    texture(constants.history_size, row.size()),
    shader("spectrogram"),
    palette{16, {
        {0.00, {0.0f, 0.0f, 0.0f}},
        {0.10, {0.0f, 0.0f, 0.5f}},
        {0.30, {1.0f, 0.0f, 1.0f}},
        {0.80, {1.0f, 1.0f, 0.0f}},
        {1.00, {1.0f, 1.0f, 1.0f}}
    }}
{}

SpectrogramVisualizer::~SpectrogramVisualizer()
{}

const std::string SpectrogramVisualizer::getTitle() const
{
    return "Spectrogram Visualizer";
}

void SpectrogramVisualizer::audioThreadFunc()
{
    SimpleRecordClient src(10 * 1000, "pulseviz", "spectrogram");
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
        this->render_mutex.lock();
        // TODO: I think this section could be optimized. A for loop looks expensive.
        for (unsigned int i = 0; i < stft.coefficients.size(); i++)
        {
            float coefficient_in_dB = STFT::convertToDecibel(stft.coefficients[i]);
            this->row[i] = (coefficient_in_dB - constants.y_min) / (constants.y_max - constants.y_min);
        }
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
    if (constants.scrolling)
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

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, start_index); glVertex2f(0.0, 0.0);
    glTexCoord2f(0.0, end_index); glVertex2f(1.0, 0.0);
    glTexCoord2f(1.0, end_index); glVertex2f(1.0, 1.0);
    glTexCoord2f(1.0, start_index); glVertex2f(0.0, 1.0);
    glEnd();

    this->shader.unbind();
}

void SpectrogramVisualizer::attachSRC()
{

    this->quit_thread = false;
    this->audio_thread = std::thread([this] {
        this->audioThreadFunc();
    });
}

void SpectrogramVisualizer::detatchSRC()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

SpectrogramVisualizerFactory::SpectrogramVisualizerFactory(const IniParser& ini)
{
    SpectrogramVisualizer::loadConfig(ini);
}

std::unique_ptr<Visualizer> SpectrogramVisualizerFactory::create() const
{
    return std::unique_ptr<Visualizer>(new SpectrogramVisualizer());
}
