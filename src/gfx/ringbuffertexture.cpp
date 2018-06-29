#include "ringbuffertexture.h"
#include <iostream>

RingBufferTexture2D::RingBufferTexture2D(std::size_t rows, std::size_t columns)
    :
    Texture2D(
        Texture::ColorFormat::LUMINANCE,
        columns,
        rows
    ),
    cursor(0),
    columns(columns), rows(rows)
{
    this->bind();
    this->setWrapMode(GL_CLAMP, GL_REPEAT);
    this->setFiltering(Texture::Filtering::BILINEAR);

    std::vector<float> texture_data(rows * columns);
    for (float& value: texture_data)
        value = 0.0;
    this->uploadData(texture_data);
}

void RingBufferTexture2D::insertRow(const std::vector<float> &row)
{
    if (row.size() != this->columns)
        throw "row does not have the correct size.";

    this->subImage(
        0,
        static_cast<GLsizei>(this->cursor),
        static_cast<GLsizei>(this->columns),
        1,
        row
    );

    this->cursor = (this->cursor + 1) % this->rows;
}

std::pair<float, float> RingBufferTexture2D::getCursorCoordinates()
{
    // TODO: Complete this picture and add documentation!
    //
    //     0.0         1.0
    // 0.0 ┌───┬───┬───┬
    //     │   │   │   │ < (end) -- because of using GL_REPEAT
    //     ├───┼───┼───┼ < cursor
    //     │   │   │   │ < start
    //     ├───┼───┼───┼
    //     │   │   │   │
    // 1.0 ├───┼───┼───┼
    //     ┆   ┆   ┆   ┆ < end
    //     └───┴───┴───┴

    float start = static_cast<float>(this->cursor) / static_cast<float>(this->rows);
    start += 0.5f * 1.0f / static_cast<float>(this->rows);
    float end = start;
    end += static_cast<float>(this->rows - 1) / static_cast<float>(this->rows);
    return std::make_pair(start, end);
}
