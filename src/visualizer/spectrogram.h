#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <GL/glew.h>
#include "../visualizer.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"
#include "../gfx/ringbuffertexture.h"
#include "../dsp/stft.h"

class SpectrogramVisualizerFactory;

class SpectrogramVisualizer : public Visualizer
{
public:
    SpectrogramVisualizer(const SpectrogramVisualizerFactory& factory);
    ~SpectrogramVisualizer() override;
    const std::string getTitle() const override;
    void draw() override;
    void resize(int width, int height) override;

private:
    const SpectrogramVisualizerFactory& factory;

    void audioThreadFunc();
    bool quit_thread;
    std::thread audio_thread;

    SimpleRecordClient src;
    STFT stft;

    std::vector<float> row;
    RingBufferTexture2D texture;

    // TODO: Rename! this should be a row_mutex.
    std::mutex render_mutex; // TODO: Move this to Visualizer base class?

    Shader shader;
    PaletteTexture palette;
};

class SpectrogramVisualizerFactory : public VisualizerFactory
{
    friend class SpectrogramVisualizer;

public:
    SpectrogramVisualizerFactory(const IniParser& ini);
    std::unique_ptr<Visualizer> create() const;

private:
    size_t fft_size, history_size;
    float dB_min, dB_max, dB_clip;
    bool scrolling;
    bool vertical;
};

#endif // SPECTROGRAM_H
