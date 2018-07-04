#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "../visualizer.h"
#include "../dsp/sampler.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"

class WaveFormVisualizer : public Visualizer
{
public:
    static struct Config
    {
        unsigned int buffer_size = 0;
    } config;
    static void loadConfig(const IniParser& ini);

    static const std::string name;

    WaveFormVisualizer();
    ~WaveFormVisualizer() override;

    const std::string& getTitle() const override;

    void attachSRC() override;
    void detatchSRC() override;
    void draw() override;

protected:
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
};

#endif // WAVEFORM_H
