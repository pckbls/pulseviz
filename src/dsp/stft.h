#ifndef STFT_H
#define STFT_H

#include "sampler.h"

enum class STFTWindow
{
    RECTANGLE,
    HAMMING
};

class STFT
{
public:
    STFT(std::shared_ptr<SimpleRecordClient> src, size_t fft_size, size_t window_size, float window_overlap, enum STFTWindow window);
    ~STFT();
    void slide();
    const std::vector<float> &getFrequencies();

    std::vector<float> coefficients;

protected:
    std::vector<float> frequencies;
    Sampler sampler;
    std::vector<float> window;
    float window_sum;
    // TODO: Use shared_ptr !!! because of copy constructor evilness!
    fftwf_plan plan;
    float *in;
    fftwf_complex *out;
};

#endif // STFT_H
