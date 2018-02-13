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
    SpectrumVisualizer();
    ~SpectrumVisualizer();
    virtual const char *getTitle();
    void render();
    void onFramebuffersizeChanged(unsigned int width, unsigned int height);

protected:
    std::vector<float> spectrum;
    STFT stft;
    std::mutex data_mutex; // TODO: Rename to spectrum_mutex
    Shader shader;
    void audioThreadFunc();
};

#endif // SPECTRUM_H
