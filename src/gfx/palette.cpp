#include "palette.h"
#include "fbo.h"
#include <iostream>

PaletteColor::PaletteColor(float r, float g, float b)
    :
    r(r), g(g), b(b)
{}

PaletteTexture::PaletteTexture(size_t width, ColorVector colors)
    :
    Texture1D(Texture::ColorFormat::RGB, width)
{
    this->bind();
    this->setFiltering(Texture::Filtering::BILINEAR);

    // TODO: This does not work yet! Do we have to manually specify a border?
    float border_color[] = {1.0f, 0.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, border_color);
    this->setWrapMode(GL_CLAMP_TO_BORDER);

    std::vector<float> data(this->getLength() * 3);

    for (unsigned int i = 0; i < data.size(); i += 3)
    {
        float pos = i / (3.0f * (this->getLength() - 1));

        // TODO: Describe!
        auto pair = this->findGradientPair(colors, pos);
        const auto& color_a = colors[pair.first];
        const auto& color_b = colors[pair.second];

        // TODO: Rename!
        float bla = (pos - color_a.first) * 1.0f / (color_b.first - color_a.first);

        // TODO: Describe!
        data[i+0] = bla * (color_b.second.r - color_a.second.r) + color_a.second.r; // Red
        data[i+1] = bla * (color_b.second.g - color_a.second.g) + color_a.second.g; // Green
        data[i+2] = bla * (color_b.second.b - color_a.second.b) + color_a.second.b; // Blue
    }

    this->uploadData(data);
}

std::pair<unsigned int, unsigned int> PaletteTexture::findGradientPair(const PaletteTexture::ColorVector& colors, float x)
{
    // TODO: assert that size(colors) > 2;
    // TODO: assert that x coordinates of ColorVector are monotonically increasing.

    unsigned int a = 0, b = 0;

    for (unsigned int i = 0; i < colors.size() - 1; i++)
    {
        if (colors[i].first <= x && x <= colors[i+1].first)
        {
            a = i;
            b = i+1;
        }
    }

    // TODO: Check if a && b are still 0.

    return std::make_pair(a, b);
}
