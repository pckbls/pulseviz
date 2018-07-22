#define _USE_MATH_DEFINES
#include <math.h> // TODO: Use <cmath> instead.
#include <iostream>
#include "bands.h"
#include "../util.h"

std::vector<BandsAnalyzer::FrequencyBin> generateOctaveBands(unsigned int fraction)
{
    std::vector<std::pair<float, float>> result;

    for (float band_number: util::linspace(-6.0f, 4.0f, 11 * fraction))
    {
        float center_frequency = std::pow(10.0f, 3.0f) * std::pow(2.0f, band_number);

        // Sources:
        // * https://en.wikipedia.org/wiki/Octave_band
        // * http://www.sengpielaudio.com/calculator-octave.htm
        float fd = std::pow(2.0f, 1.0f / (2.0f * fraction));
        std::pair<float, float> frequency_pair(center_frequency / fd, center_frequency * fd);
        result.push_back(frequency_pair);
    }

    return result;
}

BandsAnalyzer::BandsAnalyzer(STFT &stft, std::vector<FrequencyBin> frequency_bins)
    :
    stft(stft)
{
    const std::vector<float> fft_frequencies = stft.getFrequencies();
    float k = static_cast<float>(this->stft.sampler.data.size()) / static_cast<float>(this->stft.sampler.src.getSampleRate());
    float max_upper_index = static_cast<float>(fft_frequencies.size()) - 1.0f;

    for (FrequencyBin& bin: frequency_bins)
    {
        Band band;

        band.frequencies.lower = bin.first;
        band.frequencies.upper = bin.second;
        band.frequencies.center = (band.frequencies.upper + band.frequencies.lower) / 2.0f;

        band.indices.lower = static_cast<unsigned int>(util::clip(std::ceil(band.frequencies.lower * k), 0.0f, max_upper_index));
        band.indices.upper = static_cast<unsigned int>(util::clip(std::ceil(band.frequencies.upper * k), 0.0f, max_upper_index));

        this->bands.push_back(band);
    }
}

void BandsAnalyzer::tick()
{
    this->stft.slide();

    for (Band& band: this->bands)
    {
        band.magnitude = 0.0;
        for (unsigned int i = band.indices.lower; i <= band.indices.upper; i++)
            band.magnitude += this->stft.coefficients[i];
        band.magnitude /= band.indices.upper - band.indices.lower + 1; // TODO: +1?
    }
}

const std::vector<Band>& BandsAnalyzer::getBands()
{
    return this->bands;
}
