#ifndef FBO_H
#define FBO_H

#include <memory>
#include <GL/glew.h>
#include "texture.h"

class FBO
{
public:
    FBO(Texture& texture);
    ~FBO();
    void bind();
    void unbind();

protected:
    GLuint handle;
    Texture& texture;
};

#endif // FBO_H
