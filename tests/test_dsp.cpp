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

TEST(DSPTest, generateOctaveBandsTest)
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
        float center_frequency = (frequencies[i].first + frequencies[i].second) / 2.0f;
        EXPECT_NEAR(center_frequency, reference_center_frequencies[i], 0.1);
    }
}

TEST(DSPTest, STFT)
{
    SRCMock src;
    STFT stft(src, 1024, 512, 0.5, STFT::Window::RECTANGLE);

    ASSERT_EQ(stft.coefficients.size(), 513);

    auto& frequencies = stft.getFrequencies();
    ASSERT_EQ(frequencies[0], 0.0f);
    ASSERT_EQ(frequencies.back(), src.getSampleRate() / 2.0f);
}

TEST(DSPTest, BandsAnalyzer)
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

TEST(DSPTest, AWeighting)
{
    // Reference values taken from: https://www.vernier.com/til/3500/
    std::vector<float> frequencies = {31.5f, 63.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f};
    std::vector<float> reference_weightings = {-39.4f, -26.2f, -16.1f, -8.6f, -3.2f, 0.0f, 1.2f, 1.0f, -1.1f};

    for (unsigned int i = 0; i < frequencies.size(); i++)
    {
        float foo = STFT::calculateFrequencyWeighting(frequencies[i], STFT::Weighting::A);
        ASSERT_NEAR(foo, reference_weightings[i], 1.0f);
    }
}

TEST(DSPTest, ZWeighting)
{
    ASSERT_FLOAT_EQ(1.0f, STFT::calculateFrequencyWeighting(0.0, STFT::Weighting::Z));
    ASSERT_FLOAT_EQ(1.0f, STFT::calculateFrequencyWeighting(1337.0, STFT::Weighting::Z));
    ASSERT_FLOAT_EQ(1.0f, STFT::calculateFrequencyWeighting(20000.0, STFT::Weighting::Z));
}

} // namespace
