#include <iostream>
#include <memory>
#include <thread>
#include <math.h>
#include <GL/glew.h>
#include <fftw3.h>
#include "visualizer.h"

const struct
{
    float fft_size = 4096*2;
    float window_size = 1024;
    float window_overlap = 0.5;
    float y_min = -70.0;
    float y_max = 0.0;
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

SpectrumVisualizer::SpectrumVisualizer(std::shared_ptr<SimpleRecordClient> src_ptr)
    :
    Visualizer(src_ptr),
    spectrum(std::vector<float>(constants.fft_size/2+1)),
    stft(
        src_ptr,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFTWindow::HAMMING
    ),
    shader(vertex_shader_src, fragment_shader_src)
{
    this->startThread();
}

SpectrumVisualizer::~SpectrumVisualizer()
{
    this->stopThread();
}

const char* SpectrumVisualizer::getTitle()
{
    return "Spectrum Visualizer";
}

void SpectrumVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->stft.slide();

        this->data_mutex.lock();
        for (unsigned int i = 0; i < this->spectrum.size(); i++)
            this->spectrum[i] = this->stft.coefficients[i];
        this->data_mutex.unlock();
    }
}

void SpectrumVisualizer::onFramebuffersizeChanged(unsigned int /* width */, unsigned int /* height */)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, constants.y_min, constants.y_max, -1.0, 1.0);
}

void SpectrumVisualizer::render()
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
    glBindTexture(GL_TEXTURE_1D, this->colorscheme->getPaletteTexture().getHandle());

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
