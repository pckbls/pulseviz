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
} constants;

void OctavebandsVisualizer::loadConfig(const IniParser& ini)
{
    constants.fft_size = ini.getOptionAsUnsignedInteger("octavebands", "fft_size");
    constants.window_size = ini.getOptionAsUnsignedInteger("octavebands", "window_size");
    constants.n = ini.getOptionAsUnsignedInteger("octavebands", "n");
}

OctavebandsVisualizer::OctavebandsVisualizer()
    :
    Visualizer(),
    bars(generateOctaveBands(constants.n).size()), // TODO: meh..
    shader("octavebands"),
    palette{16, {
        {0.0, {1.0, 0.0, 0.0}},
        {0.5, {1.0, 0.0, 1.0}},
        {0.6, {1.0, 1.0, 1.0}},
        {1.0, {1.0, 1.0, 1.0}},
    }}
{
    for (Bar& bar: this->bars)
    {
        bar.height = constants.y_min;
        bar.tick_height = constants.y_min;
    }
}

OctavebandsVisualizer::~OctavebandsVisualizer()
{}

const std::string OctavebandsVisualizer::getTitle() const
{
    return "Octave Bands Visualizer";
}

void OctavebandsVisualizer::audioThreadFunc()
{
    SimpleRecordClient src(10 * 1000, "pulseviz", "octavebands");
    STFT stft(
        src,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFT::Window::HAMMING
    );
    BandsAnalyzer bands_analyzer(stft, generateOctaveBands(constants.n), BandWeighting::Z);

    while (!this->quit_thread)
    {
        bands_analyzer.tick();
        this->render_mutex.lock();
        unsigned int i = 0;
        for (Bar &bar: this->bars)
        {
            bar.height = STFT::convertToDecibel(bands_analyzer.getBands()[i].magnitude);
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
        bar.tick_height -= constants.tick_velocity * (float)duration.count();
        if (bar.tick_height < bar.height)
            bar.tick_height = bar.height;
    }
}

void OctavebandsVisualizer::draw()
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

    // TODO: Initialize in constructor
    const float bars_width = (1.0 - (this->bars.size() - 1) * constants.bar_spacing) / this->bars.size();

    this->updateTicks();

    this->render_mutex.lock();

    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 0);
    this->palette.bind();

    unsigned int i = 0;
    for (Bar &bar: this->bars)
    {
        // TODO: Initialize in constructor
        float x_min = i * bars_width + i * constants.bar_spacing;
        float x_max = (i + 1) * bars_width + i * constants.bar_spacing;

        float magnitude = (bar.height - constants.y_min) / (constants.y_max - constants.y_min);
        magnitude *= 1.0;

#if 1
        this->shader.bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex2f(x_min, constants.y_min);
        glVertex2f(x_max, constants.y_min);
        glTexCoord2f(magnitude, magnitude);
        glVertex2f(x_max, bar.height);
        glVertex2f(x_min, bar.height);
        glEnd();
        this->shader.unbind();
#else
        // TODO: Render GL_POINTS for testing purposes
#endif

        glLineWidth(2.0);
        glBegin(GL_LINES);
        auto bg_color = PaletteColor(0.0f, 0.0f, 0.0f); // TODO: Remove!
        glColor3f(1.0f - bg_color.r, 1.0f - bg_color.g, 1.0f - bg_color.b);
        glVertex2f(x_min, bar.tick_height);
        glVertex2f(x_max, bar.tick_height);
        glEnd();

        i += 1;
    }

    this->render_mutex.unlock();
}

void OctavebandsVisualizer::attachSRC()
{

    this->quit_thread = false;
    this->audio_thread = std::thread([this] {
        this->audioThreadFunc();
    });
}

void OctavebandsVisualizer::detatchSRC()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

OctavebandsVisualizerFactory::OctavebandsVisualizerFactory(const IniParser& ini)
{
    OctavebandsVisualizer::loadConfig(ini);
}

std::unique_ptr<Visualizer> OctavebandsVisualizerFactory::create() const
{
    return std::unique_ptr<Visualizer>(new OctavebandsVisualizer());
}
