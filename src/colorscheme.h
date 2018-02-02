#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <memory>
#include "gfx/palette.h"

class ColorScheme
{
    using PaletteColorVector = std::vector<std::pair<float, PaletteColor>>;

public:
    ColorScheme(PaletteColor background_color, PaletteColorVector palette_colors);
    void createPaletteTexture();
    const PaletteColor& getBackgroundColor();
    PaletteTexture &getPaletteTexture();

protected:
    PaletteColor background_color;
    PaletteColorVector palette_colors;
    std::shared_ptr<PaletteTexture> palette_texture;
};

#endif // COLORSCHEME_H
