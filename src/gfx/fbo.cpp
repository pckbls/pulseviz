#include "fbo.h"

FBO::FBO(Texture& texture)
    :
    handle(0),
    texture(texture)
{
    glGenFramebuffers(1, &this->handle);

    this->bind();

    switch (this->texture.getTarget())
    {
        case GL_TEXTURE_1D:
            glFramebufferTexture1D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_1D,
                this->texture.getHandle(),
                0
            );
            break;

        default:
            throw "Unknown texture target.";
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw "Cannot create texture framebuffer";
}

FBO::~FBO()
{
    this->unbind();
    glDeleteFramebuffers(1, &this->handle);
}

void FBO::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->handle);
}

void FBO::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
