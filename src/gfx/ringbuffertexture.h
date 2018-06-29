#ifndef RINGBUFFERTEXTURE_H
#define RINGBUFFERTEXTURE_H

#include "texture2d.h"
#include <vector>
#include <utility>

class RingBufferTexture2D : public Texture2D
{
public:
    // TODO: Order is confusing!
    RingBufferTexture2D(std::size_t rows, std::size_t columns);
    void insertRow(const std::vector<float>& row);
    std::pair<float, float> getCursorCoordinates();

private:
    unsigned int cursor;
    std::size_t columns, rows;
};

#endif // RINGBUFFERTEXTURE_H
