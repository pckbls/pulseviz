#pragma once

#include "texture.h"

class Texture2D : public Texture
{
public:
    Texture2D(ColorFormat color_format, size_t width, size_t height);
    Texture2D(const Texture2D&) = delete;

    size_t getWidth() const;
    size_t getHeight() const;

    void uploadData(const std::vector<float>& data);
    void subImage(int x, int y,
                  int width, int height,
                  const std::vector<float>& data);

    virtual void setFiltering(Texture::Filtering filtering) const override;
    void setWrapMode(GLint x, GLint y);

    GLenum getTarget() const override;

private:
    size_t width, height;
};
