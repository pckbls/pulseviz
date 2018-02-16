#include "texture2d.h"
#include <iostream>

template<TextureColorFormat T>
Texture2D<T>::Texture2D(size_t width, size_t height)
    :
    Texture<T>(),
    width(width), height(height)
{}

template<TextureColorFormat T>
void Texture2D<T>::uploadData(const std::vector<float>& data)
{
    if (data.size() != this->width * this->height * 1)
        throw "Size does not match."; // TODO: Proper exception message!

    this->bind();

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        this->getColorFormatAsGLuint(),
        this->width,
        this->height,
        0,
        this->getColorFormatAsGLuint(),
        GL_FLOAT,
        data.data()
    );
}

template<TextureColorFormat T>
size_t Texture2D<T>::getWidth() const
{
    return this->width;
}

template<TextureColorFormat T>
size_t Texture2D<T>::getHeight() const
{
    return this->height;
}

template<TextureColorFormat T>
void Texture2D<T>::subImage(unsigned int x, unsigned int y,
                            size_t width, size_t height,
                            std::vector<float>& data)
{
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        x, y,
        width, height,
        this->getColorFormatAsGLuint(),
        GL_FLOAT,
        &data
    );
}

template<TextureColorFormat T>
void Texture2D<T>::setTextureFiltering(TextureFiltering filtering)
{
    if ((filtering == TextureFiltering::TRILINEAR)
        && (this->width % 2 != 0 || this->height % 2 != 0))
    {
        std::cerr << this->width << " " << this->height << std::endl;
        throw "Mipmap generation requires both texture width and height to be multiples of 2";
    }

    Texture<T>::setTextureFiltering(filtering);
}

template<TextureColorFormat T>
void Texture2D<T>::setTextureWrapMode(GLuint x, GLuint y)
{
    glTexParameteri(this->getTarget(), GL_TEXTURE_WRAP_S, x);
    glTexParameteri(this->getTarget(), GL_TEXTURE_WRAP_T, y);
}

template<TextureColorFormat T>
GLuint Texture2D<T>::getTarget()
{
    return GL_TEXTURE_2D;
}

template class Texture2D<TextureColorFormat::Luminance>;
template class Texture2D<TextureColorFormat::RGB>;
