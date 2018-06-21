#ifndef RINGBUFFERTEXTURE_H
#define RINGBUFFERTEXTURE_H

#include <vector>
#include "texture2d.h"

class RingBufferTexture2D : public Texture2D<TextureColorFormat::Luminance>
{
public:
    // TODO: Order is confusing!
    RingBufferTexture2D(std::size_t rows, std::size_t columns);
    void insertRow(const std::vector<float>& row);
    float getCursorCoordinate();

protected:
    unsigned int insert_index;
    std::size_t columns, rows;
};

#endif // RINGBUFFERTEXTURE_H
