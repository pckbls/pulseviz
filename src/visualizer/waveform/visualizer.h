#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "../../visualizer.h"
#include "../../dsp/sampler.h"
#include "../../gfx/shader.h"
#include "../../gfx/palette.h"

class WaveFormVisualizer : public Visualizer
{
public:
    WaveFormVisualizer();
    ~WaveFormVisualizer();
    virtual const char *getTitle();
    void render();

protected:
    void audioThreadFunc();
    std::mutex mutex;
    Sampler sampler;
    Shader shader;
};

#endif // WAVEFORM_H
