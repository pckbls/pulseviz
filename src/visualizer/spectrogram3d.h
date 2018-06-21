#ifndef SPECTROGRAM_3D_H
#define SPECTROGRAM_3D_H

#include <GL/glew.h>
#include "../visualizer.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"
#include "../gfx/ringbuffertexture.h"
#include "../dsp/stft.h"

// TODO: Inherit from SpectrogramVisualizer to reduce code redundancy!
class Spectrogram3DVisualizer : public Visualizer
{
public:
    static void loadConfig(const IniParser& ini);
    static const std::string name;

    Spectrogram3DVisualizer();
    ~Spectrogram3DVisualizer() override;

    const std::string& getName() const override;
    const std::string& getTitle() const override;

    void attachSRC() override;
    void detatchSRC() override;
    void draw() override;
    void resize(int width, int height) override;

protected:
    void audioThreadFunc();
    bool quit_thread;
    std::thread audio_thread;

    void drawPlane(float y);
    void rotate();

    std::vector<float> row;
    RingBufferTexture2D texture;

    // TODO: Rename! this should be a row_mutex.
    std::mutex render_mutex; // TODO: Move this to Visualizer base class?

    Shader shader;
    PaletteTexture palette;

    static const size_t grid_rows = 400;
    static const size_t grid_columns = 200;
    float grid[grid_rows][grid_columns];

    float last_start_index;

    struct
    {
        float x, y, z;
    } rotation;
};

#endif // SPECTROGRAM_3D_H
