#pragma once

#include <vector>
#include <GL/glew.h>

class Texture
{
public:
    enum class ColorFormat
    {
        LUMINANCE,
        RGB
    };

    enum class Filtering
    {
        NEAREST,
        BILINEAR,
        TRILINEAR
    };

    Texture(ColorFormat color_format);
    Texture(const Texture&) = delete;
    virtual ~Texture();

    GLuint getHandle() const;
    void bind(unsigned int texture_unit = 0) const;
    void unbind() const;
    virtual void setFiltering(Filtering filtering) const;

    virtual GLenum getTarget() const = 0;

protected:
    GLint internal_format;
    GLenum format;
    size_t pixel_size;

private:
    GLuint handle;
};
