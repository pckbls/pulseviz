#include "texture.h"

Texture::Texture(Texture::ColorFormat color_format)
    :
    handle(0)
{
    switch (color_format)
    {
        case ColorFormat::LUMINANCE:
            this->internal_format = GL_LUMINANCE;
            this->format = GL_LUMINANCE;
            this->pixel_size = 1;
            break;

        case ColorFormat::RGB:
            this->internal_format = GL_RGB;
            this->format = GL_RGB;
            this->pixel_size = 3;
            break;
    }

    glGenTextures(1, &this->handle);
}

Texture::~Texture()
{
    glDeleteTextures(1, &this->handle);
}

void Texture::bind(unsigned int texture_unit) const
{
    // TODO: Check result!
    // See: https://www.khronos.org/registry/OpenGL-Refpages/es1.1/xhtml/glActiveTexture.xml
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(this->getTarget(), this->handle);
}

void Texture::unbind() const
{
    glBindTexture(this->getTarget(), 0);
}

GLuint Texture::getHandle() const
{
    return this->handle;
}

void Texture::setFiltering(Texture::Filtering filtering) const
{
    this->bind();
    GLenum target = this->getTarget();
    switch (filtering)
    {
        case Filtering::NEAREST:
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;

        case Filtering::BILINEAR:
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;

        case Filtering::TRILINEAR:
            this->bind();
            glGenerateMipmap(this->getTarget());

            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE); // TODO: Legacy, remove!
            break;
    }
    this->unbind();
}
