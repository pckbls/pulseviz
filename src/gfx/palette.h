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
// TODO: Make it possible to specify colors for over/underrange!

class PaletteTexture : public Texture1D
{
    // TODO: Proper naming! (Or delete)
    using ColorVector = std::vector<std::pair<float, PaletteColor>>;

public:
    // TODO: Use reference
    // TODO: The width parameter is useless!
    PaletteTexture(size_t width, ColorVector colors);

private:
    // TODO: Find a more suitable name!
    std::pair<unsigned int, unsigned int> findGradientPair(const ColorVector& colors, float x);
};

#endif // PALETTE_H
