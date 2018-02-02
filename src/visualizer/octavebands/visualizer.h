#ifndef OCTAVEBANDS_H
#define OCTAVEBANDS_H

#include <chrono>
#include <fftw3.h>
#include "../../dsp/bands.h"
#include "../../visualizer.h"
#include "../../gfx/shader.h"
#include "../../gfx/palette.h"

struct Bar
{
    float height;
    float tick_height;
};

class OctavebandsVisualizer : public Visualizer
{
public:
    OctavebandsVisualizer(std::shared_ptr<SimpleRecordClient> src_ptr);
    ~OctavebandsVisualizer();
    virtual const char *getTitle();
    void render();
    void onFramebuffersizeChanged(unsigned int width, unsigned int);

protected:
    void audioThreadFunc();
    void updateTicks();

    STFT stft;
    BandsAnalyzer bands_analyzer;
    std::vector<Bar> bars;
    std::mutex render_mutex;
    Shader shader;
    std::chrono::steady_clock::time_point last_render_tp;
};

#endif // SPECTRUM_H
