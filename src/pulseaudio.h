#ifndef PULSEAUDIO_H
#define PULSEAUDIO_H

#include <exception>
#include <string>
#include <vector>
#include "pulse/simple.h"
#include <pulse/error.h>

struct PulseAudioException : std::exception
{
    PulseAudioException(const std::string &what, int error);
    const char* what() const noexcept;
    std::string message;
    int pa_error;
};

class SimpleRecordClient
{
public:
    // TODO: Re-order arguments!
    SimpleRecordClient(pa_usec_t target_latency,
                       const std::string &name, const std::string &stream_name,
                       unsigned int sample_rate = 44100, uint8_t channels = 1);
    SimpleRecordClient(SimpleRecordClient const& other) = delete;
    ~SimpleRecordClient();

    pa_usec_t getLatency();
    unsigned int getSampleRate();

    void read(std::vector<float> &data);
    void flush();

private:
    pa_simple *pa_handle;
};

#endif // PULSEAUDIO_H
