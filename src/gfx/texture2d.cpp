#include "texture2d.h"
#include <iostream>

Texture2D::Texture2D(ColorFormat color_format, size_t width, size_t height)
    :
    Texture(color_format),
    width(width), height(height)
{
    // Initially fill the texture with black ink.
    std::vector<float> texture_data(this->width * this->height * this->pixel_size);
    for (float& value: texture_data)
        value = 0.0;
    this->uploadData(texture_data);
}

size_t Texture2D::getWidth() const
{
    return this->width;
}

size_t Texture2D::getHeight() const
{
    return this->height;
}

void Texture2D::uploadData(const std::vector<float>& data)
{
    if (data.size() != this->width * this->height * this->pixel_size)
        throw "data vector has the wrong size.";

    this->bind();
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        this->internal_format,
        static_cast<GLsizei>(this->width),
        static_cast<GLsizei>(this->height),
        0,
        this->format,
        GL_FLOAT,
        data.data()
    );
    this->unbind();
}

void Texture2D::subImage(int x, int y, int width, int height, const std::vector<float>& data)
{
    // TODO: Check size of data!

    this->bind();
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        x, y,
        width, height,
        this->format,
        GL_FLOAT,
        data.data()
    );
    this->unbind();
}

void Texture2D::setFiltering(Texture::Filtering filtering) const
{
    if ((filtering == Texture::Filtering::TRILINEAR)
        && (this->width % 2 != 0 || this->height % 2 != 0))
    {
        throw "Mipmap generation requires both texture width and height to be multiples of 2";
    }

    Texture::setFiltering(filtering);
}

void Texture2D::setWrapMode(GLint x, GLint y)
{
    this->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, x);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, y);
    this->unbind();
}

GLenum Texture2D::getTarget() const
{
    return GL_TEXTURE_2D;
}
