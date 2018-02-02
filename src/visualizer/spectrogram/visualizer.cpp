// https://www.tuxboard.com/photos/2013/11/Ondulations-Modem-spectogramme.jpg
// https://media.licdn.com/mpr/mpr/AAEAAQAAAAAAAAiKAAAAJDY1ZTQxMGE1LWIxZTAtNGZkNS1hY2U1LWFkMWM5N2JkN2MxZQ.jpg
// http://nickrossiter.org.uk/hbweb/24%20hb%20juv%2021%20sept%202005%20south%20tyne%20long%20flight%20call.jpg
// https://stackoverflow.com/a/13248668
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
// https://www.opengl.org/discussion_boards/showthread.php/184634-Sharing-textures-between-contexts
// https://www.youtube.com/watch?v=vvr9AMWEU-c

#include <iostream>
#include "visualizer.h"
#include "constants.h"

static const char* vertex_shader_src = R"glsl(
    #version 120

    varying vec2 tex_coord;

    void main()
    {
        tex_coord = gl_MultiTexCoord0.xy;
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    }
)glsl";

static const char* fragment_shader_src = R"glsl(
    #version 120

    varying vec2 tex_coord;

    uniform sampler2D texture;
    uniform sampler1D palette;

    void main(void)
    {
        vec2 texcoord = tex_coord;

        // TODO: Proper log scaling!
        texcoord.x = pow(texcoord.x, 2.0);

        if (texcoord.x > 1.0)
        {
            // This should not happen!
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
        else
        {
            // gl_FragColor = vec4(texcoord.x, texcoord.y, 0.0, 1.0);

            // gl_FragColor = texture1D(palette, texture2D(texture, texcoord));

            float magnitude = texture2D(texture, texcoord).x;
            if (magnitude < 0.0 || magnitude > 1.0)
            {
                // This should not happen
                gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
            }
            else
                gl_FragColor = texture1D(palette, magnitude);
        }
    }
)glsl";

SpectrogramVisualizer::SpectrogramVisualizer(std::shared_ptr<SimpleRecordClient> src_ptr)
    :
    Visualizer(src_ptr),
    stft(
        src_ptr,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFTWindow::HAMMING
    ),
    texture(this->stft, 2048),
    shader(vertex_shader_src, fragment_shader_src)
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
