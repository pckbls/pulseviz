#include "colorscheme.h"

ColorScheme::ColorScheme(PaletteColor background_color, PaletteColorVector palette_colors)
    :
    background_color(background_color),
    palette_colors(palette_colors),
    palette_texture(nullptr)
{}

void ColorScheme::createPaletteTexture()
{
    const unsigned int palette_width = 32;
    // TODO: This call correct?
    this->palette_texture = std::make_shared<PaletteTexture>(
        PaletteTexture(palette_width, this->palette_colors)
    );
}

const PaletteColor& ColorScheme::getBackgroundColor()
{
    return this->background_color;
}

PaletteTexture& ColorScheme::getPaletteTexture()
{
    // TODO: Are we doing this right?
    return *this->palette_texture.get();
}
