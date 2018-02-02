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

    float weight;
    float magnitude;

    struct
    {
        unsigned int lower, upper;
    } indices;
};

enum class BandWeighting
{
    Z
};

std::vector<std::pair<float, float>> generateOctaveBands(unsigned int fraction);

class BandsAnalyzer
{
public:
    BandsAnalyzer(STFT& stft, std::vector<std::pair<float, float>> frequency_bins, BandWeighting band_weighting);
    void tick();
    const std::vector<Band>& getBands();

protected:
    STFT& stft;
    std::vector<Band> bands;
};

#endif // BANDS_H
