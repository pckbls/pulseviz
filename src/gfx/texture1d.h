#ifndef TEXTURE1D_H
#define TEXTURE1D_H

#include "texture.h"

template<TextureColorFormat T>
class Texture1D : public Texture<T>
{
public:
    Texture1D(size_t length);
    void uploadData(const std::vector<float>& data);
    size_t getLength() const;
    void setTextureWrapMode(GLuint x);

protected:
    GLuint getTarget() override;
    size_t length;
};

#endif // TEXTURE1D_H
