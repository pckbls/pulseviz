#include "texture.h"
#include <iostream>

template<TextureColorFormat T>
Texture<T>::Texture()
    :
    shared_handle(new GLuint(0))
{
    glGenTextures(1, this->shared_handle.get());
}

template<TextureColorFormat T>
Texture<T>::~Texture()
{
    if (this->shared_handle.use_count() > 1)
        return;

    glDeleteTextures(1, this->shared_handle.get());
}

template<TextureColorFormat T>
GLuint Texture<T>::getHandle()
{
    return *this->shared_handle;
}

template<TextureColorFormat T>
void Texture<T>::setTextureFiltering(TextureFiltering filtering)
{
    if (filtering == TextureFiltering::NEAREST)
    {
        glTexParameteri(this->getTarget(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(this->getTarget(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else if (filtering == TextureFiltering::BILINEAR)
    {
        glTexParameteri(this->getTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(this->getTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (filtering == TextureFiltering::TRILINEAR)
    {
        this->bind();
        glGenerateMipmap(this->getTarget());

        glTexParameteri(this->getTarget(), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(this->getTarget(), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(this->getTarget(), GL_GENERATE_MIPMAP, GL_TRUE); // TODO: Legacy, remove!
    }
    else
        throw "Unknown filtering type";
}

template<>
GLuint Texture<TextureColorFormat::Luminance>::getColorFormatAsGLuint()
{
    return GL_LUMINANCE;
}

template<>
GLuint Texture<TextureColorFormat::RGB>::getColorFormatAsGLuint()
{
    return GL_RGB;
}

template<TextureColorFormat T>
void Texture<T>::bind()
{
    glBindTexture(this->getTarget(), *this->shared_handle);
}

template<TextureColorFormat T>
void Texture<T>::unbind()
{
    glBindTexture(this->getTarget(), 0);
}

// TODO: Can we do better than that?!...
// See: https://bytefreaks.net/programming-2/c/c-undefined-reference-to-templated-class-function
template class Texture<TextureColorFormat::Luminance>;
template class Texture<TextureColorFormat::RGB>;
