#include "texture1d.h"

Texture1D::Texture1D(Texture::ColorFormat color_format, size_t length)
    :
    Texture(color_format),
    length(length)
{
    this->bind();

    // Initially fill the texture with black ink.
    std::vector<float> texture_data(this->length * this->pixel_size);
    for (float& value: texture_data)
        value = 0.0;
    this->uploadData(texture_data);

    this->unbind();
}

void Texture1D::uploadData(const std::vector<float>& data)
{
    if (data.size() != this->length * this->pixel_size)
        throw "data vector has the wrong size.";

    this->bind();
    glTexImage1D(
        this->getTarget(),
        0,
        this->internal_format,
        static_cast<GLsizei>(this->length),
        0,
        this->format,
        GL_FLOAT,
        data.data()
    );
    this->unbind();
}

void Texture1D::setWrapMode(GLint x)
{
    this->bind();
    glTexParameteri(this->getTarget(), GL_TEXTURE_WRAP_S, x);
    this->unbind();
}

size_t Texture1D::getLength() const
{
    return this->length;
}

GLenum Texture1D::getTarget() const
{
    return GL_TEXTURE_1D;
}
