#include <iostream>
#include "sampler.h"

Sampler::Sampler(SimpleRecordClient& src, size_t data_size, size_t chunk_size)
    :
    data(data_size),
    chunk(chunk_size),
    src(src)
{
    // TODO: flush or drain?
    this->src.flush();
}

void Sampler::readChunk()
{
    // TODO: Explain blocking behavior!
    this->src.read(this->chunk);
}

void Sampler::appendToBuffer()
{
    this->data.erase(std::begin(this->data), std::begin(this->data) + this->chunk.size());
    this->data.insert(std::end(this->data), std::begin(this->chunk), std::end(this->chunk));
}
