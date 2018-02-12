#include "visualizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

const struct
{
    float fft_size = 4096;
    float window_size = 1024;
    float window_overlap = 0.5;
    float y_min = -70.0;
    float y_max = 0.0;
    bool scrolling = true;
} constants;

static const char* vertex_shader_src = R"glsl(
    #version 120

    uniform sampler2D texture;
    attribute int is_spectrogram_vertex;
    varying float magnitude;

    void main()
    {
        vec2 tex_coord = gl_MultiTexCoord0.xy;

        // TODO: Proper log scaling!
        tex_coord.x = pow(tex_coord.x, 3.0);

        vec4 position = gl_Vertex;
        //if (is_spectrogram_vertex > 0)
        if (true)
        {
            magnitude = texture2D(texture, tex_coord).x;
            position.z = magnitude * 1.3;
        }

        gl_Position = gl_ModelViewProjectionMatrix * position;
    }
)glsl";

static const char* fragment_shader_src = R"glsl(
    #version 120

    varying float magnitude;

    uniform sampler1D palette;

    void main(void)
    {
        if (magnitude > 1.0)
        {
            // This should not happen!
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
        else
        {
            // gl_FragColor = vec4(texcoord.x, texcoord.y, 0.0, 1.0);

            // gl_FragColor = texture1D(palette, texture2D(texture, texcoord));

            if (magnitude < 0.0 || magnitude > 1.0)
            {
                // This should not happen
                gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
                gl_FragColor = texture1D(palette, 0.0);
            }
            else
                gl_FragColor = texture1D(palette, magnitude);
        }
    }
)glsl";

Spectrogram3DVisualizer::Spectrogram3DVisualizer()
    :
    Visualizer(),
    stft(
        this->src,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFTWindow::HAMMING
    ),
    texture(this->stft, this->grid_rows),
    shader(vertex_shader_src, fragment_shader_src),
    last_start_index(0.0)
{
    for (unsigned int i = 0; i < this->grid_rows; i++)
        for (unsigned int j = 0; j < this->grid_columns; j++)
            this->grid[i][j] = 0.0;

    this->rotation.x = 55.0;
    this->rotation.y = 0.0;
    this->rotation.z = 35.0;

    this->startThread();
}

Spectrogram3DVisualizer::~Spectrogram3DVisualizer()
{
    this->stopThread();
}

const char* Spectrogram3DVisualizer::getTitle()
{
    return "Spectrogram 3D Visualizer";
}

void Spectrogram3DVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->stft.slide();
        this->render_mutex.lock();
        this->texture.updateTextureData();
        this->render_mutex.unlock();
    }
}

void Spectrogram3DVisualizer::onFramebuffersizeChanged(unsigned int width, unsigned int height)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4x4 matrix = glm::perspective(5.0f, (float)width / (float)height, 1.0f, 1000.0f);
    glLoadMatrixf(glm::value_ptr(matrix));
}

void Spectrogram3DVisualizer::drawPlane(float y) {
    const float foo = 10.0;
    glColor4f(0.1, 0.1, 0.1, 0.5);
    glBegin(GL_QUADS);
    glVertex3f(-foo, y, -foo);
    glVertex3f(+foo, y, -foo);
    glVertex3f(+foo, y, +foo);
    glVertex3f(-foo, y, +foo);
    glEnd();
}

void Spectrogram3DVisualizer::render()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->render_mutex.lock();

    this->texture.updateTexture();

    this->rotation.z = 0.0;
    this->rotation.z = 0.0;
    this->rotation.x = 80.0;

    float start_index = 0.0;
    if (constants.scrolling)
    {
        // TODO: Explain the cleverness!
        start_index = (float)this->texture.getCursorPosition() / (float)this->texture.getHistorySize();
        float column_width = 1.0f / (float)this->texture.getHistorySize();
        start_index += 0.5f * column_width;

#if 0
        if (start_index - last_start_index > 1.0f / (float)this->grid_rows || start_index < last_start_index)
            last_start_index = start_index;
        else
            start_index = last_start_index;
#endif

        // start_index += 1.0f / ((float)this->grid_rows - 1); // TODO

        // TODO: Snap start_index to multiples of 1/this->grid_rows? This might solve the jerkiness issues.
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, -0.5f, -24.0f);
    glRotatef(this->rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(this->rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(this->rotation.z, 0.0f, 0.0f, 1.0f);

    glPointSize(3.0f);
    glLineWidth(2.0f);

    this->shader.bind();
    glActiveTexture(GL_TEXTURE0 + 0);
    glUniform1i(this->shader.getUniformLocation("texture"), 0);
    glBindTexture(GL_TEXTURE_2D, this->texture.getHandle());

    glActiveTexture(GL_TEXTURE1 + 0);
    glUniform1i(this->shader.getUniformLocation("palette"), 1);
    glBindTexture(GL_TEXTURE_1D, this->colorscheme->getPaletteTexture().getHandle());

    for (unsigned int i = 0; i < this->grid_rows; i++)
    {
        glBegin(GL_LINE_STRIP);
        for (unsigned int j = 0; j < this->grid_columns; j++)
        {
            // TODO: Watch out for off-by-one-errors
            glTexCoord2f(
                1.0f - (float)j / (float)this->grid_columns,
                start_index + (float)i / (float)this->grid_rows
            );
            glVertex3f(
                -5.0f + 10.0f * (float)j / (float)this->grid_columns,
                -20.0f + 40.0f * (float)i / (float)this->grid_rows,
                this->grid[i][j]
            );
        }
        glEnd();
    }

    this->shader.unbind();

    if (!constants.scrolling)
    {
        // TODO: Really +1?
        float y = (float)(this->texture.getCursorPosition() + 1) / (float)this->texture.getHistorySize();
        y = -5.0f + 10.0f * y;
        //this->drawPlane(y);
    }

    this->render_mutex.unlock();

    glDisable(GL_DEPTH_TEST);
}
