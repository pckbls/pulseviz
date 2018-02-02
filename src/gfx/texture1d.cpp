#include "texture1d.h"

template<TextureColorFormat T>
Texture1D<T>::Texture1D(size_t length)
    :
    Texture<T>(),
    length(length)
{
    this->setup();

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
void Texture1D<T>::setParameters()
{
    // TODO: Is this really stored per texture? Otherwise the function should be global.
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

template<TextureColorFormat T>
void Texture1D<T>::bind()
{
    glBindTexture(GL_TEXTURE_1D, *this->shared_handle);
}

template<TextureColorFormat T>
void Texture1D<T>::unbind()
{
    glBindTexture(GL_TEXTURE_1D, 0);
}

template class Texture1D<TextureColorFormat::Luminance>;
template class Texture1D<TextureColorFormat::RGB>;
