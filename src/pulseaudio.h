#ifndef AUDIO_H
#define AUDIO_H

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
    SimpleRecordClient(pa_usec_t target_latency, const std::string &name, const std::string &stream_name);
    SimpleRecordClient(SimpleRecordClient const& other);
    ~SimpleRecordClient();

    pa_usec_t getLatency();
    unsigned int getSampleRate();

    void read(std::vector<float> &data);
    void flush();

private:
    // TODO: Use smart pointer!
    pa_simple *pa_handle;
};

#endif // AUDIO_H
