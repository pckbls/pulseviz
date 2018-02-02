#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <GL/glew.h>
#include "../../visualizer.h"
#include "../../gfx/shader.h"
#include "../../dsp/stft.h"
#include "../../dsp/spectrogramtexture.h"

class SpectrogramVisualizer : public Visualizer
{
public:
    SpectrogramVisualizer(std::shared_ptr<SimpleRecordClient> src_ptr);
    ~SpectrogramVisualizer();
    virtual const char *getTitle();
    void render();
    void onFramebuffersizeChanged(unsigned int width, unsigned int height);

protected:
    void audioThreadFunc();

    STFT stft;
    SpectrogramTexture texture;
    Shader shader;
    std::mutex render_mutex; // TODO: Move this to Visualizer base class?
};

#endif // SPECTROGRAM_H
