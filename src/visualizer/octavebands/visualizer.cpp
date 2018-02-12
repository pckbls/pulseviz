#include <iostream>
#include <memory>
#include <thread>
#include <random>
#include <math.h>
#include <GL/glew.h>
#include <fftw3.h>
#include "visualizer.h"

const struct
{
    float fft_size = 4096*2;
    float window_size = 1024;
    float window_overlap = 0.5;
    unsigned int n = 3;
    float y_min = -70.0;
    float y_max = 0.0;
    float tick_velocity = 0.000000025;
    float bar_spacing = 0.0025;
} constants;

static const char* vertex_shader_src = R"glsl(
    #version 120

    varying float magnitude;

    void main()
    {
        magnitude = gl_MultiTexCoord0.y;
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    }
)glsl";

static const char* fragment_shader_src = R"glsl(
    #version 120

    varying float magnitude;

    uniform sampler1D palette;

    void main(void)
    {
        // gl_FragColor = vec4(texcoord.x, texcoord.y, 0.0, 1.0);
        // gl_FragColor = texture1D(palette, texture2D(texture, texcoord));

        if (magnitude < 0.0 || magnitude > 1.0)
        {
            // This should not happen
            gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
        else
            gl_FragColor = texture1D(palette, magnitude);
    }
)glsl";

OctavebandsVisualizer::OctavebandsVisualizer()
    :
    Visualizer(),
    stft(
        this->src,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFTWindow::HAMMING
    ),
    bands_analyzer(this->stft, generateOctaveBands(constants.n), BandWeighting::Z),
    bars(this->bands_analyzer.getBands().size()),
    shader(vertex_shader_src, fragment_shader_src)
{
    for (Bar& bar: this->bars)
    {
        bar.height = constants.y_min;
        bar.tick_height = constants.y_min;
    }

    this->startThread();
}

OctavebandsVisualizer::~OctavebandsVisualizer()
{
    this->stopThread();
}

const char* OctavebandsVisualizer::getTitle()
{
    return "Octavebands Visualizer";
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

void OctavebandsVisualizer::onFramebuffersizeChanged(unsigned int /* width */, unsigned int /* height */)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, constants.y_min, constants.y_max, -1.0, 1.0);
}

void OctavebandsVisualizer::updateTicks()
{
    std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - this->last_render_tp;

    for (Bar &bar: this->bars)
    {
        bar.tick_height -= constants.tick_velocity * (float)duration.count();
        if (bar.tick_height < bar.height)
            bar.tick_height = bar.height;
    }
}

void OctavebandsVisualizer::render()
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
    this->colorscheme->getPaletteTexture().bind();
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

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
        auto& bg_color = this->colorscheme->getBackgroundColor();
        glColor3f(1.0f - bg_color.r, 1.0f - bg_color.g, 1.0f - bg_color.b);
        glVertex2f(x_min, bar.tick_height);
        glVertex2f(x_max, bar.tick_height);
        glEnd();

        i += 1;
    }

    this->render_mutex.unlock();

    // TODO: Move to visualizer base class
    this->last_render_tp = std::chrono::steady_clock::now();
}
