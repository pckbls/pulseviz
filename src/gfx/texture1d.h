#pragma once

#include "texture.h"

class Texture1D : public Texture
{
public:
    Texture1D(ColorFormat color_format, size_t length);
    Texture1D(const Texture1D&) = delete;

    size_t getLength() const;
    void uploadData(const std::vector<float>& data);
    void setWrapMode(GLint x);
    GLenum getTarget() const override;

private:
    size_t length;
};
