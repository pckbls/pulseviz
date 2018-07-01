#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <fftw3.h>
#include "../visualizer.h"
#include "../dsp/stft.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"

class SpectrumVisualizer : public Visualizer
{
public:
    static void loadConfig(const IniParser& ini);
    static const std::string name;

    SpectrumVisualizer();
    ~SpectrumVisualizer() override;

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

    std::vector<float> spectrum;
    std::mutex data_mutex; // TODO: Rename to spectrum_mutex

    Shader shader;
    PaletteTexture palette;
};

class SpectrumVisualizerFactory : public VisualizerFactory
{
public:
    SpectrumVisualizerFactory(const IniParser& ini);
    std::unique_ptr<Visualizer> create() const;
};

#endif // SPECTRUM_H
