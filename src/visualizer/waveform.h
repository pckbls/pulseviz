#pragma once

#include "../visualizer.h"
#include "../dsp/sampler.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"

class WaveFormVisualizer : public Visualizer
{
public:
    WaveFormVisualizer(size_t buffer_size);
    ~WaveFormVisualizer() override;
    const std::string getTitle() const override;
    void resize(int width, int height) override;
    void draw() override;

private:
    void audioThreadFunc();
    bool quit_thread;
    std::thread audio_thread;

    std::vector<float> samples;
    std::mutex mutex;

    Shader shader;
    PaletteTexture palette;
};

class WaveFormVisualizerFactory : public VisualizerFactory
{
public:
    WaveFormVisualizerFactory(const IniParser& ini);
    std::unique_ptr<Visualizer> create() const;

private:
    size_t buffer_size;
};
