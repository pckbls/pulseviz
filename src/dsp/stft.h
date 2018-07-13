#pragma once

#include "sampler.h"

class BandsAnalyzer;

class STFT
{
public:
    friend class BandsAnalyzer;

    enum class Window
    {
        RECTANGLE,
        HAMMING
    };

    enum class Weighting
    {
        A,
        Z
    };

    static std::vector<float> generateRectangleWindow(unsigned int n);
    static std::vector<float> generateHammingWindow(unsigned int n);
    static float convertToDecibel(float magnitude); // TODO: Remove this! Or move back from util.cpp?
    static float calculateFrequencyWeighting(float frequency, STFT::Weighting weighting);

    STFT(SimpleRecordClient& src,
         size_t sample_size,
         size_t window_size, float window_overlap,
         STFT::Window window, STFT::Weighting = STFT::Weighting::Z);
    STFT(STFT const& other) = delete;
    ~STFT();

    void slide();
    const std::vector<float> &getFrequencies() const;
    float getBinWidth();

    std::vector<float> coefficients; // TODO: Make private

protected:
    constexpr static const float min_dB = -200.0; // TODO: Is this the right way to define a static constant?

    size_t sample_size;
    std::vector<float> frequencies;
    Sampler sampler;
    std::vector<float> window;
    float window_sum;
    fftwf_plan plan;
    float *in;
    fftwf_complex *out;

private:

    std::vector<float> frequency_weights;
};
