#ifndef SPECTROGRAM_3D_H
#define SPECTROGRAM_3D_H

#include <GL/glew.h>
#include "../visualizer.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"
#include "../gfx/ringbuffertexture.h"
#include "../dsp/stft.h"

class Spectrogram3DVisualizerFactory;

// TODO: Inherit from SpectrogramVisualizer to reduce code redundancy!
class Spectrogram3DVisualizer : public Visualizer
{
public:
    Spectrogram3DVisualizer(const Spectrogram3DVisualizerFactory& factory);
    ~Spectrogram3DVisualizer() override;
    const std::string getTitle() const override;
    void draw() override;
    void resize(int width, int height) override;

private:
    void audioThreadFunc();
    bool quit_thread;
    std::thread audio_thread;

    void drawPlane(float y);
    void rotate();

    SimpleRecordClient src;
    STFT stft;

    std::vector<float> row;
    RingBufferTexture2D texture;

    // TODO: Rename! this should be a row_mutex.
    std::mutex render_mutex; // TODO: Move this to Visualizer base class?

    Shader shader;
    PaletteTexture palette;

    static const size_t grid_rows = 400;
    static const size_t grid_columns = 200;
    float grid[grid_rows][grid_columns];

    struct
    {
        float x, y, z;
    } rotation;
};

class Spectrogram3DVisualizerFactory : public VisualizerFactory
{
    friend class Spectrogram3DVisualizer;

public:
    Spectrogram3DVisualizerFactory(const IniParser& ini);
    std::unique_ptr<Visualizer> create() const;

private:
    size_t fft_size;
    float dB_min, dB_max, dB_clip;
};

#endif // SPECTROGRAM_3D_H
