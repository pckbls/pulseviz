#include "gtest/gtest.h"
#include "../src/pulseaudio.h"

namespace
{

TEST(PulseAudioTest, readSamplesPositive)
{
    SimpleRecordClient src(10 * 1000, "foo", "bar");

    ASSERT_NO_THROW(src.getSampleRate());
    ASSERT_NO_THROW(src.getLatency());

    std::vector<float> data(512);
    ASSERT_NO_THROW(src.read(data));

    ASSERT_NO_THROW(src.flush());
}

TEST(PulseAudioTest, readSamplesNegative)
{
    SimpleRecordClient src(10 * 1000, "foo", "bar");
    std::vector<float> data;
    ASSERT_THROW(src.read(data), std::runtime_error);
}

} // namespace
