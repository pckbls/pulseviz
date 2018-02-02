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
void Texture<T>::setup()
{
    this->bind();
    this->setParameters();
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

template<>
GLuint Texture<TextureColorFormat::Luminance>::getColorFormatAsGLuint() {
    return GL_LUMINANCE;
}

template<>
GLuint Texture<TextureColorFormat::RGB>::getColorFormatAsGLuint() {
    return GL_RGB;
}

template<TextureColorFormat T>
void Texture<T>::bind()
{
    throw "Not implemented.";
}

template<TextureColorFormat T>
void Texture<T>::unbind()
{
    throw "Not implemented.";
}

template<TextureColorFormat T>
void Texture<T>::setParameters()
{
    throw "Not implemented.";
}

// TODO: Can we do better than that?!...
// See: https://bytefreaks.net/programming-2/c/c-undefined-reference-to-templated-class-function
template class Texture<TextureColorFormat::Luminance>;
template class Texture<TextureColorFormat::RGB>;
