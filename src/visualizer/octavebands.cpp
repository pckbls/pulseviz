#include <iostream>
#include <memory>
#include <thread>
#include <random>
#include <math.h>
#include <GL/glew.h>
#include <fftw3.h>
#include "octavebands.h"

struct
{
    unsigned int fft_size = 4096*2;
    unsigned int window_size = 1024;
    float window_overlap = 0.5;
    unsigned int n = 3;
    float y_min = -70.0;
    float y_max = 0.0;
    float tick_velocity = 0.000000025;
    float bar_spacing = 0.0025;
    bool enable_ticks;
} constants;

void OctavebandsVisualizer::loadConfig(const IniParser& ini)
{
    constants.fft_size = ini.getOptionAsUnsignedInteger("fft", "fft_size");
    constants.window_size = ini.getOptionAsUnsignedInteger("fft", "window_size");
    constants.n = ini.getOptionAsUnsignedInteger("octavebands", "n");
    constants.bar_spacing = ini.getOptionAsFloat("octavebands", "spacing");
    constants.enable_ticks = ini.getOptionAsBool("octavebands", "ticks");
}

OctavebandsVisualizer::OctavebandsVisualizer()
    :
    Visualizer(),
    src(10 * 1000, "pulseviz", "octavebands"),
    stft(
        src,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFT::Window::HAMMING,
        STFT::Weighting::Z
    ),
    bands_analyzer(stft, generateOctaveBands(constants.n)),
    bars(bands_analyzer.getBands().size()),
    shader("octavebands"),
    palette{16, {
        {0.0, {0.0, 0.0, 0.75}},
        {0.5, {1.0, 0.0, 0.75}},
        {1.0, {1.0, 1.0, 1.0}},
    }}
{
    for (Bar& bar: this->bars)
    {
        bar.height = constants.y_min;
        bar.tick_height = constants.y_min;
    }

    this->quit_thread = false;
    this->audio_thread = std::thread([this] {
        this->audioThreadFunc();
    });
}

OctavebandsVisualizer::~OctavebandsVisualizer()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

const std::string OctavebandsVisualizer::getTitle() const
{
    return "Octave Bands Visualizer";
}

void OctavebandsVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->bands_analyzer.tick();
        this->render_mutex.lock();
        unsigned int i = 0;
        for (Bar &bar: this->bars)
        {
            bar.height = this->bands_analyzer.getBands()[i].magnitude;
            i += 1;
        }
        this->render_mutex.unlock();
    }
}

void OctavebandsVisualizer::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, constants.y_min, constants.y_max, -1.0, 1.0);
}

void OctavebandsVisualizer::updateTicks()
{
    // TODO: Initialize last_render_tp with something useful!
    auto duration = std::chrono::steady_clock::now() - this->last_render_tp;
    this->last_render_tp = std::chrono::steady_clock::now();

    for (Bar &bar: this->bars)
    {
        bar.tick_height -= constants.tick_velocity * static_cast<float>(duration.count());
        if (bar.tick_height < bar.height)
            bar.tick_height = bar.height;
    }
}

void OctavebandsVisualizer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Initialize in constructor
    const float bars_width = (1.0f - (this->bars.size() - 1) * constants.bar_spacing) / this->bars.size();

    this->updateTicks();

    this->render_mutex.lock();

    unsigned int i = 0;
    for (Bar &bar: this->bars)
    {
        // TODO: Initialize in constructor
        float x_min = i * bars_width + i * constants.bar_spacing;
        float x_max = (i + 1) * bars_width + i * constants.bar_spacing;

        float magnitude = (bar.height - constants.y_min) / (constants.y_max - constants.y_min);
        magnitude *= 1.0;

        this->shader.bind();
        glActiveTexture(GL_TEXTURE0 + 0);
        glUniform1i(this->shader.getUniformLocation("palette"), 0);
        this->palette.bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(x_min, constants.y_min);
        glVertex2f(x_max, constants.y_min);
        glTexCoord2f(magnitude, magnitude);
        glVertex2f(x_max, bar.height);
        glVertex2f(x_min, bar.height);
        glEnd();
        this->shader.unbind();

        if (constants.enable_ticks)
        {
            glLineWidth(2.0);
            glBegin(GL_LINES);
            auto bg_color = PaletteColor(0.0f, 0.0f, 0.0f); // TODO: Remove!
            glColor3f(1.0f - bg_color.r, 1.0f - bg_color.g, 1.0f - bg_color.b);
            glVertex2f(x_min, bar.tick_height);
            glVertex2f(x_max, bar.tick_height);
            glEnd();
        }

        i += 1;
    }

    this->render_mutex.unlock();
}

OctavebandsVisualizerFactory::OctavebandsVisualizerFactory(const IniParser& ini)
{
    OctavebandsVisualizer::loadConfig(ini);
}

std::unique_ptr<Visualizer> OctavebandsVisualizerFactory::create() const
{
    auto* visualizer = new OctavebandsVisualizer();
    return std::unique_ptr<Visualizer>(visualizer);
}
