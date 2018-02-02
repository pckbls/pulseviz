#ifndef FBO_H
#define FBO_H

#include <memory>
#include <GL/glew.h>
#include "texture1d.h"

// TODO: Rename to FBO1D?
class FBO
{
public:
    FBO(Texture1D<TextureColorFormat::RGB> &texture);
    ~FBO();
    void bind();
    void unbind();

protected:
    GLuint handle;
    Texture1D<TextureColorFormat::RGB>& texture;
};

#endif // FBO_H
