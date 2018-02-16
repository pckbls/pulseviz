#include "palette.h"
#include "fbo.h"
#include <iostream>

// TODO: Use GL_CLAMP, see: https://wiki.delphigl.com/index.php/glTexParameter
// TODO: Use GL_CLAMP_TO_BORDER and set specific "error" color using GL_TEXTURE_BORDER_COLOR

PaletteColor::PaletteColor(float r, float g, float b)
    :
    r(r), g(g), b(b)
{}

PaletteTexture::PaletteTexture(size_t width, ColorVector colors):
    Texture1D(width)
{
    this->bind();
    this->setTextureFiltering(TextureFiltering::BILINEAR);
    this->setTextureWrapMode(GL_CLAMP_TO_EDGE);

    // Save the current viewport state.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glMatrixMode(GL_MODELVIEW); glPushMatrix();
    glMatrixMode(GL_PROJECTION); glPushMatrix();

    // Now fill up the palette with meaningful data.
    FBO fbo(*this);
    fbo.bind();
    this->render(colors);
    fbo.unbind();

    // Restore the previous viewport state.
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void PaletteTexture::render(ColorVector &colors)
{
    glViewport(0, 0, this->length, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1, 1);

    glLineWidth(1.0);
    glBegin(GL_LINE_STRIP);
    for (auto pair: colors)
    {
        float& x = pair.first;
        PaletteColor& color = pair.second;
        glColor3f(color.r, color.g, color.b);
        glVertex2f(x, 0.5f);
    }
    glEnd();
}
