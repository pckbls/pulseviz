#ifndef SPECTROGRAM_3D_H
#define SPECTROGRAM_3D_H

#include <GL/glew.h>
#include "../visualizer.h"
#include "../gfx/shader.h"
#include "../dsp/stft.h"
#include "../dsp/spectrogramtexture.h"

class Spectrogram3DVisualizer : public Visualizer
{
public:
    Spectrogram3DVisualizer();
    ~Spectrogram3DVisualizer();
    const char *getTitle() override;
    void onFramebuffersizeChanged(unsigned int width, unsigned int height) override;
    void render() override;

protected:
    void audioThreadFunc() override;
    void drawPlane(float y);
    void rotate();

    STFT stft;
    SpectrogramTexture texture;
    Shader shader;
    std::mutex render_mutex; // TODO: Move this to Visualizer base class?

    static const size_t grid_rows = 400;
    static const size_t grid_columns = 200;
    float grid[grid_rows][grid_columns];

    float last_start_index;

    struct
    {
        float x, y, z;
    } rotation;
};

#endif // SPECTROGRAM_3D_H
