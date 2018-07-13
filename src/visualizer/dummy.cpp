#include <GL/glew.h>
#include "dummy.h"

static const std::string vertex_shader_src = R"glsl(
    #version 120

    varying float magnitude;

    void main()
    {
        magnitude = gl_MultiTexCoord0.x;
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    }
)glsl";

static const std::string fragment_shader_src = R"glsl(
    #version 120

    varying float magnitude;
    uniform sampler1D palette;

    void main(void)
    {
        gl_FragColor = texture1D(palette, magnitude);
    }
)glsl";

DummyVisualizerNew::DummyVisualizerNew()
    :
    palette_a{4, {
        {0.0, {0.0, 0.0, 0.0}},
        {1.0, {1.0, 1.0, 1.0}},
    }},
    palette_b{32, {
        {0.00, {0.0f, 0.0f, 0.0f}},
        {0.10, {0.0f, 0.0f, 0.5f}},
        {0.30, {1.0f, 0.0f, 1.0f}},
        {0.80, {1.0f, 1.0f, 0.0f}},
        {1.00, {1.0f, 1.0f, 1.0f}}
    }},
    shader(vertex_shader_src, fragment_shader_src)
{}

const std::string DummyVisualizerNew::getTitle() const
{
    return "Dummy Visualizer";
}

void DummyVisualizerNew::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1, 1);
}

void DummyVisualizerNew::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_1D);

    this->shader.bind();

    this->palette_a.bind(0);
    glUniform1i(this->shader.getUniformLocation("palette"), 0);
    glBegin(GL_QUADS);
    glTexCoord1f(-0.1f); glVertex2f(0.0f, 0.0f);
    glTexCoord1f(-0.1f); glVertex2f(0.5f, 0.0f);
    glTexCoord1f(1.1f); glVertex2f(0.5f, 1.0f);
    glTexCoord1f(1.1f); glVertex2f(0.0f, 1.0f);
    glEnd();
    this->palette_a.unbind();

    this->palette_b.bind(0);
    glUniform1i(this->shader.getUniformLocation("palette"), 0);
    glBegin(GL_QUADS);
    glTexCoord1f(-0.1f); glVertex2f(0.5f, 0.0f);
    glTexCoord1f(-0.1f); glVertex2f(1.0f, 0.0f);
    glTexCoord1f(1.1f); glVertex2f(1.0f, 1.0f);
    glTexCoord1f(1.1f); glVertex2f(0.5f, 1.0f);
    glEnd();
    this->palette_b.unbind();

    this->shader.unbind();
}

std::unique_ptr<Visualizer> DummyVisualizerFactory::create() const
{
    return std::unique_ptr<Visualizer>(new DummyVisualizerNew());
}
