#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <GL/glew.h>
#include "../visualizer.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"
#include "../gfx/ringbuffertexture.h"
#include "../dsp/stft.h"

class SpectrogramVisualizer : public Visualizer
{
public:
    static void loadConfig(const IniParser& ini);
    static const std::string name;

    SpectrogramVisualizer();
    ~SpectrogramVisualizer() override;

    const std::string& getTitle() const override;

    void attachSRC() override;
    void detatchSRC() override;
    void draw() override;
    void resize(int width, int height) override;

protected:
    void audioThreadFunc();
    bool quit_thread;
    std::thread audio_thread;

    std::vector<float> row;
    RingBufferTexture2D texture;

    // TODO: Rename! this should be a row_mutex.
    std::mutex render_mutex; // TODO: Move this to Visualizer base class?

    Shader shader;
    PaletteTexture palette;
};

class SpectrogramVisualizerFactory : public VisualizerFactory
{
public:
    SpectrogramVisualizerFactory(const IniParser& ini);
    std::unique_ptr<Visualizer> create() const;
};

#endif // SPECTROGRAM_H
