#ifndef BANDS_H
#define BANDS_H

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <fftw3.h>
#include "stft.h"

// TOOD: Move this to BandsAnalyzer?
struct Band
{
    struct
    {
        float lower, center, upper;
    } frequencies;

    float magnitude;

    struct
    {
        unsigned int lower, upper;
    } indices;
};

class BandsAnalyzer
{
public:
    using FrequencyBin = std::pair<float, float>;

    BandsAnalyzer(STFT& stft, std::vector<FrequencyBin> frequency_bins);
    BandsAnalyzer(BandsAnalyzer const& other) = delete;
    void tick(); // TODO: Find a better functuion name...
    const std::vector<Band>& getBands();

protected:
    STFT& stft;
    std::vector<Band> bands;
};

std::vector<BandsAnalyzer::FrequencyBin> generateOctaveBands(unsigned int fraction);

#endif // BANDS_H
