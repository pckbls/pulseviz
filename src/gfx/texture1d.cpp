#include "texture1d.h"

template<TextureColorFormat T>
Texture1D<T>::Texture1D(size_t length)
    :
    Texture<T>(),
    length(length)
{
    this->bind();

    std::vector<float> texture_data(this->length * 3);
    for (float& value: texture_data)
        value = 0.0;
    this->uploadData(texture_data);
}

template<TextureColorFormat T>
void Texture1D<T>::uploadData(const std::vector<float>& data)
{
    if (data.size() != this->length * 3)
        throw "Size does not match."; // TODO: Proper exception message!

    this->bind();

    glTexImage1D(
        GL_TEXTURE_1D,
        0,
        this->getColorFormatAsGLuint(),
        this->length,
        0,
        this->getColorFormatAsGLuint(),
        GL_FLOAT,
        data.data()
    );
}

template<TextureColorFormat T>
size_t Texture1D<T>::getLength() const
{
    return this->length;
}

template<TextureColorFormat T>
void Texture1D<T>::setTextureWrapMode(GLuint x)
{
    glTexParameteri(this->getTarget(), GL_TEXTURE_WRAP_S, x);
}

template<TextureColorFormat T>
GLuint Texture1D<T>::getTarget()
{
    return GL_TEXTURE_1D;
}

template class Texture1D<TextureColorFormat::Luminance>;
template class Texture1D<TextureColorFormat::RGB>;
