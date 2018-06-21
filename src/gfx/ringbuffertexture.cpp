#include "ringbuffertexture.h"
#include <iostream>

RingBufferTexture2D::RingBufferTexture2D(std::size_t rows, std::size_t columns)
    :
    Texture2D(columns, rows),
    insert_index(0),
    columns(columns), rows(rows)
{
    this->bind();
    this->setTextureWrapMode(GL_CLAMP, GL_REPEAT); // TODO: Does OpenGL remember that setting?
    this->setTextureFiltering(TextureFiltering::BILINEAR);

    std::vector<float> texture_data(rows * columns);
    for (float& value: texture_data)
        value = 0.0;
    this->uploadData(texture_data);
}

void RingBufferTexture2D::insertRow(const std::vector<float> &row)
{
    if (row.size() != this->columns)
    {
        std::cerr << row.size() << " " << this->columns;
        throw "Nope!"; // TODO
    }

    glEnable(this->getTarget());
    this->bind();

    glTexSubImage2D(
        this->getTarget(), 0,
        0, this->insert_index,
        this->columns, 1,
        GL_LUMINANCE, GL_FLOAT,
        row.data()
    );

    this->unbind();
    glDisable(this->getTarget());

    this->insert_index = (this->insert_index + 1) % this->rows;
}

float RingBufferTexture2D::getCursorCoordinate()
{
    // TODO: Check for off-by-one error, the right edge looks weird!
    return static_cast<float>(this->insert_index) / static_cast<float>(this->rows);
}
