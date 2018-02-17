#ifndef SPECTROGRAMTEXTURE_H
#define SPECTROGRAMTEXTURE_H

#include <GL/glew.h>
#include "stft.h"
#include "../gfx/shader.h"
#include "../gfx/texture2d.h"

class SpectrogramTexture : public Texture2D<TextureColorFormat::Luminance>
{
public:
    SpectrogramTexture(STFT& stft, size_t history_size);
    SpectrogramTexture(SpectrogramTexture const& other) = delete;
    ~SpectrogramTexture();
    void updateTextureData();
    void updateTexture();
    unsigned int getCursorPosition();
    size_t getHistorySize();

protected:
    STFT& stft;
    size_t history_size;
    std::vector<float> texture_data;
    bool texture_contents_updated;
    unsigned int insert_index;
    unsigned int new_columns_start_index;
};

#endif // SPECTROGRAMTEXTURE_H
