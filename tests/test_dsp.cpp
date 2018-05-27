#include "gtest/gtest.h"
#include "../src/dsp/stft.h"
#include "../src/dsp/bands.h"
#include "../src/pulseaudio.h"

class SRCMock : public SimpleRecordClient
{
public:
    SRCMock()
        :
        SimpleRecordClient(1337, "foo", "bar")
    {}

    void read(std::vector<float>& /* data */)
    {}
};

class STFTMock : public STFT
{
public:
    STFTMock(SRCMock& src, size_t fft_size)
        :
        STFT(src, fft_size, fft_size, 0.0, STFT::Window::RECTANGLE)
    {}

    void slide()
    {}
};

namespace
{

TEST(DSP, generateOctaveBandsTest)
{
    // Reference values taken from:
    // http://www.engineeringtoolbox.com/octave-bands-frequency-limits-d_1602.html
    std::vector<float> reference_center_frequencies = {
        16.0, 31.5, 63.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0, 16000.0
    };

    auto frequencies = generateOctaveBands(1);
    ASSERT_EQ(frequencies.size(), reference_center_frequencies.size());

    for (size_t i = 0; i < frequencies.size(); i++)
    {
        float center_frequency = (frequencies[i].first + frequencies[i].second) / 2.0;
        EXPECT_NEAR(center_frequency, reference_center_frequencies[i], 0.1);
    }
}

TEST(DSP, STFT)
{
    SRCMock src;
    STFT stft(src, 1024, 512, 0.5, STFT::Window::RECTANGLE);
    EXPECT_EQ(stft.coefficients.size(), 512);
}

TEST(DSP, BandsAnalyzer)
{
    SRCMock src;
    STFTMock stft(src, 4);
    std::vector<BandsAnalyzer::FrequencyBin> frequency_bins = {
        {0.0, 1.0},
        {1.0, 2.0}
    };
    BandsAnalyzer bands(stft, frequency_bins, BandWeighting::Z);

    stft.coefficients = {
        1.0, 2.0, 3.0, 4.0
    };
    bands.tick();

    std::vector<float> expected_magnitudes = {
        0.0, 0.0
    };

    ASSERT_EQ(bands.getBands().size(), expected_magnitudes.size());
    for (unsigned int i = 0; i < expected_magnitudes.size(); i++)
        ASSERT_EQ(bands.getBands()[i].magnitude, expected_magnitudes[i]);
}

} // namespace
