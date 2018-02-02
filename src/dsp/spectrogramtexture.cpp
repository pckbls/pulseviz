#include "spectrogramtexture.h"

// TODO: Add function that maps frequency/time onto texture coordinates

const struct
{
    float y_min = -70.0;
    float y_max = 0.0;
} constants;

SpectrogramTexture::SpectrogramTexture(STFT &stft, size_t history_size)
    :
    Texture2D(stft.coefficients.size(), history_size),
    stft(stft),
    history_size(history_size),
    texture_data(stft.coefficients.size() * history_size),
    texture_contents_updated(false),
    insert_index(0),
    new_columns_start_index(0)
{
    for (float& value: this->texture_data)
        value = 0.0;

    this->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // TOOD
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // TODO
    this->setTextureFiltering(TextureFiltering::BILINEAR);
    this->uploadData(this->texture_data);
}

SpectrogramTexture::~SpectrogramTexture()
{}

void SpectrogramTexture::updateTextureData()
{
    for (unsigned int column = 0; column < this->stft.coefficients.size(); column++)
    {
        float magnitude = (this->stft.coefficients[column] - constants.y_min) / (constants.y_max - constants.y_min);
        size_t offset = this->insert_index * this->stft.coefficients.size() + column;
        this->texture_data[offset] = magnitude;
    }

    if (!this->texture_contents_updated)
    {
        this->new_columns_start_index = this->insert_index;
        this->texture_contents_updated = true;
    }
    this->insert_index = (this->insert_index + 1) % this->history_size;
}

void SpectrogramTexture::updateTexture()
{
    glEnable(GL_TEXTURE_2D);
    this->bind();

    unsigned int row_start, rows;
    size_t data_offset;
    if (this->insert_index <= this->new_columns_start_index)
    {
        // TODO: Properly handle wrap-around, this here is too slow!
        row_start = 0;
        rows = this->history_size;
        data_offset = 0;
    }
    else
    {
        row_start = this->new_columns_start_index;
        rows = this->insert_index - this->new_columns_start_index;
        data_offset = row_start * this->stft.coefficients.size();
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, row_start,
                    this->stft.coefficients.size(), rows,
                    GL_LUMINANCE, GL_FLOAT,
                    &this->texture_data.data()[data_offset]);

    // TODO: Unbind?
    glDisable(GL_TEXTURE_2D);
    this->texture_contents_updated = false;
}

unsigned int SpectrogramTexture::getCursorPosition()
{
    return this->insert_index;
}

size_t SpectrogramTexture::getHistorySize()
{
    return this->history_size;
}
