#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <fftw3.h>
#include "../visualizer.h"
#include "../dsp/stft.h"
#include "../gfx/shader.h"
#include "../gfx/palette.h"

class SpectrumVisualizerFactory;

class SpectrumVisualizer : public Visualizer
{
public:
    SpectrumVisualizer(const SpectrumVisualizerFactory& factory);
    ~SpectrumVisualizer() override;
    const std::string getTitle() const override;
    void draw() override;
    void resize(int width, int height) override;

private:
    const SpectrumVisualizerFactory& factory;

    float frequency2xCoordinate(float frequency) const;
    void drawGrid() const;

    void audioThreadFunc();
    bool quit_thread;
    std::thread audio_thread;

    SimpleRecordClient src;
    STFT stft;

    std::vector<float> spectrum;
    std::mutex data_mutex; // TODO: Rename to spectrum_mutex

    Shader shader;
    PaletteTexture palette;
};

class SpectrumVisualizerFactory : public VisualizerFactory
{
    friend class SpectrumVisualizer;

public:
    SpectrumVisualizerFactory(const IniParser& ini);
    std::unique_ptr<Visualizer> create() const;

private:
    size_t fft_size;
    float dB_min, dB_max, dB_clip;
    bool fill;
    bool enable_grid;
};

#endif // SPECTRUM_H
