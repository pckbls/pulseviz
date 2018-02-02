#ifndef PALETTE_H
#define PALETTE_H

#include <vector>
#include <memory>
#include <GL/glew.h>
#include "texture1d.h"

struct PaletteColor
{
    PaletteColor(float r, float g, float b);
    float r, g, b;
};

// TODO: Use typedef oder alias oder was auch immer für den std::vector<...> kram unten.

class PaletteTexture : public Texture1D<TextureColorFormat::RGB>
{
    // TODO: Proper naming! (Or delete)
    using ColorVector = std::vector<std::pair<float, PaletteColor>>;

public:
    // TODO: Use reference
    PaletteTexture(size_t width, ColorVector colors);

protected:
    void render(ColorVector& colors);
};

#endif // PALETTE_H
