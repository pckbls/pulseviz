#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <memory>
#include <GL/glew.h>

// TODO: Move inside class!
enum class TextureColorFormat
{
    RGB,
    Luminance
};

// TODO: Move inside class!
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
    void bind();
    void unbind();
    GLuint getHandle();
    void setTextureFiltering(TextureFiltering filtering);

protected:
    GLuint getColorFormatAsGLuint();
    virtual GLuint getTarget() = 0; // TODO: Make this constexpr?

    // TODO: Disable the copy constructor then we can use
    // a unique_ptr instead of a shared_ptr
    std::shared_ptr<GLuint> shared_handle;
};

#endif // TEXTURE_H
