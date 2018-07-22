#include "spectrogram3d.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

const struct
{
    unsigned int fft_size = 4096;
    unsigned int window_size = 1024;
    float window_overlap = 0.5;
    float y_min = -70.0;
    float y_max = 0.0;
    float width = 10.0; // frequency axis
    float length = 10.0; // time axis
    bool scrolling = true;
} constants;

Spectrogram3DVisualizer::Spectrogram3DVisualizer(const Spectrogram3DVisualizerFactory& factory)
    :
    Visualizer(),
    src(10 * 1000, "pulseviz", "spectrogram3d"),
    stft(
        this->src,
        constants.fft_size,
        constants.window_size,
        constants.window_overlap,
        STFT::Window::HAMMING,
        STFT::Weighting::Z
    ),
    row((constants.fft_size / 2) + 1),
    texture(this->grid_rows, row.size()),
    shader("spectrogram3d"),
    palette{16, {
        {0.00, {0.0f, 0.0f, 0.0f}},
        {0.10, {0.0f, 0.0f, 0.5f}},
        {0.30, {1.0f, 0.0f, 1.0f}},
        {0.80, {1.0f, 1.0f, 0.0f}},
        {1.00, {1.0f, 1.0f, 1.0f}}
    }}
{
    (void)factory;

    for (unsigned int i = 0; i < this->grid_rows; i++)
        for (unsigned int j = 0; j < this->grid_columns; j++)
            this->grid[i][j] = 0.0;

    this->rotation.z = 0.0;
    this->rotation.z = 0.0;
    this->rotation.x = 80.0;

    this->quit_thread = false;
    this->audio_thread = std::thread([this] {
        this->audioThreadFunc();
    });
}

Spectrogram3DVisualizer::~Spectrogram3DVisualizer()
{
    this->quit_thread = true;
    this->audio_thread.join();
}

const std::string Spectrogram3DVisualizer::getTitle() const
{
    return "Spectrogram3D Visualizer";
}

void Spectrogram3DVisualizer::audioThreadFunc()
{
    while (!this->quit_thread)
    {
        this->stft.slide();
        this->render_mutex.lock();
        // TODO: I think this section could be optimized. A for loop looks expensive.
        for (unsigned int i = 0; i < this->stft.coefficients.size(); i++)
        {
            float coefficient_in_dB = this->stft.coefficients[i];
            this->row[i] = (coefficient_in_dB - constants.y_min) / (constants.y_max - constants.y_min);
        }
        this->render_mutex.unlock();
    }
}

void Spectrogram3DVisualizer::drawPlane(float y) {
    glColor4f(0.1, 0.1, 0.1, 0.5);
    glBegin(GL_QUADS);
    glVertex3f(-constants.width, y, -constants.width);
    glVertex3f(+constants.width, y, -constants.width);
    glVertex3f(+constants.width, y, +constants.width);
    glVertex3f(-constants.width, y, +constants.width);
    glEnd();
}

void Spectrogram3DVisualizer::rotate()
{
#if 0
    auto duration = this->durationSinceLastUpdate();
    this->rotation.z += duration.count() * 0.00000001;
#endif
}

void Spectrogram3DVisualizer::resize(int width, int height)
{
    glViewport(0, 0, width, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4x4 matrix = glm::perspective(5.0f, (float)width / (float)height, 1.0f, 1000.0f);
    glLoadMatrixf(glm::value_ptr(matrix));
}

void Spectrogram3DVisualizer::draw()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_2D);

#if 0
    this->rotate();
#endif

    this->render_mutex.lock();
    this->texture.insertRow(this->row);
    this->render_mutex.unlock();

    float start_index;
    if (constants.scrolling)
    {
        auto coordinates = this->texture.getCursorCoordinates();
        start_index = coordinates.first;
    }
    else
    {
        start_index = 0.0f;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 1.0f, -12.0f);
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
    glBindTexture(GL_TEXTURE_1D, this->palette.getHandle());

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
                -(constants.width / 2.0f) + constants.width * (float)j / (float)this->grid_columns,
                -(constants.length / 2.0f) + constants.length * (float)i / (float)this->grid_rows,
                this->grid[i][j]
            );
        }
        glEnd();
    }

    this->shader.unbind();

#if 0
    if (!constants.scrolling)
    {
        // TODO: Really +1?
        float y = (float)(this->texture.getCursorPosition() + 1) / (float)this->texture.getHistorySize();
        y = -(constants.length / 2.0f) + constants.length * y;
        this->drawPlane(y);
    }
#endif

    glDisable(GL_DEPTH_TEST);
}

Spectrogram3DVisualizerFactory::Spectrogram3DVisualizerFactory(const IniParser& ini)
{
    this->dB_min = ini.getOptionAsFloat("general", "dB_min");
    this->dB_max = ini.getOptionAsFloat("general", "dB_max");
    this->dB_clip = ini.getOptionAsFloat("general", "dB_clip");
    this->fft_size = ini.getOptionAsUnsignedInteger("fft", "fft_size");
}

std::unique_ptr<Visualizer> Spectrogram3DVisualizerFactory::create() const
{
    auto* visualizer = new Spectrogram3DVisualizer(*this);
    return std::unique_ptr<Visualizer>(visualizer);
}
