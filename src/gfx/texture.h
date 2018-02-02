#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <memory>
#include <GL/glew.h>

enum class TextureColorFormat
{
    RGB,
    Luminance
};

enum class TextureFiltering
{
    NEAREST,
    BILINEAR,
    TRILINEAR
};

template<TextureColorFormat T>
class Texture
{
public:
    Texture();
    virtual ~Texture();
    virtual void bind();
    virtual void unbind();
    GLuint getHandle();

protected:
    virtual void setParameters(); // TODO: Remove
    void setup(); // TODO: Remove
    GLuint getColorFormatAsGLuint();

    std::shared_ptr<GLuint> shared_handle;
};

#endif // TEXTURE_H
