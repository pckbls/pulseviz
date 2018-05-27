#ifndef SAMPLER_H
#define SAMPLER_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <fftw3.h>
#include "../pulseaudio.h"

class BandsAnalyzer;
class STFT;

class Sampler
{
    friend class STFT;
    friend class BandsAnalyzer;

public:
    Sampler(SimpleRecordClient& src, size_t data_size, size_t chunk_size);
    Sampler(Sampler const& other) = delete;
    void readChunk();
    void appendToBuffer();

    std::vector<float> data;

protected:
    std::vector<float> chunk;
    SimpleRecordClient& src;
};

#endif // SAMPLER_H
