#include "texture2d.h"
#include <iostream>

template<TextureColorFormat T>
Texture2D<T>::Texture2D(size_t width, size_t height)
    :
    Texture<T>(),
    width(width), height(height)
{
    this->setup();
}

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
void Texture2D<T>::bind()
{
    glBindTexture(GL_TEXTURE_2D, *this->shared_handle);
}

template<TextureColorFormat T>
void Texture2D<T>::unbind()
{
    glBindTexture(GL_TEXTURE_1D, 0);
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
void Texture2D<T>::setParameters()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    // TODO: GL_TEXTURE_1D/2D based on Texture Class type
    if (filtering == TextureFiltering::NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    else if (filtering == TextureFiltering::BILINEAR)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (filtering == TextureFiltering::TRILINEAR)
    {
        if (this->width % 2 != 0 || this->height % 2 != 0)
        {
            std::cerr << this->width << " " << this->height << std::endl;
            throw "Mipmap generation requires both texture width and height to be multiples of 2";
        }

        this->bind();
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // TODO: Legacy, remove!
    }
    else
        throw "Unknown filtering type";
}

template class Texture2D<TextureColorFormat::Luminance>;
template class Texture2D<TextureColorFormat::RGB>;
