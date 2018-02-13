#include <iostream>
#include "pulseaudio.h"

PulseAudioException::PulseAudioException(const std::string& what, int error)
    :
    std::exception(),
    pa_error(error)
{
    this->message = std::string(what) + ": " + pa_strerror(error);
}

const char* PulseAudioException::what() const noexcept
{
    return this->message.c_str();
}

SimpleRecordClient::SimpleRecordClient(pa_usec_t target_latency,
                                       const std::string& name,
                                       const std::string& stream_name)
    :
    pa_handle(nullptr)
{
    pa_sample_spec sample_spec;
    sample_spec.format = PA_SAMPLE_FLOAT32;
    sample_spec.rate = 44100;
    sample_spec.channels = 1;

    pa_buffer_attr buffer_attr;
    buffer_attr.maxlength = -1;
    buffer_attr.minreq = -1;
    buffer_attr.prebuf = -1;
    buffer_attr.tlength = -1;
    buffer_attr.fragsize = pa_usec_to_bytes(target_latency, &sample_spec);

    int error;
    this->pa_handle = pa_simple_new(
        NULL, name.c_str(), PA_STREAM_RECORD,
        NULL, stream_name.c_str(), &sample_spec,
        NULL, &buffer_attr, &error
    );
    if (!this->pa_handle)
        throw PulseAudioException("pa_simple_new() failed", error);
}

SimpleRecordClient::~SimpleRecordClient()
{
    this->flush();
    pa_simple_free(this->pa_handle);
}

pa_usec_t SimpleRecordClient::getLatency()
{
    int error;
    pa_usec_t result = pa_simple_get_latency(this->pa_handle, &error);
    if (!result) // TODO: Is this correct?
        throw PulseAudioException("pa_simple_get_latency() failed", error);
    return result;
}

unsigned int SimpleRecordClient::getSampleRate()
{
    return 44100;
}

void SimpleRecordClient::read(std::vector<float>& data)
{
    if (data.size() == 0)
        throw std::runtime_error("Vector size cannot be 0."); // TODO: Is the exception type correct?

    int error;
    int result = pa_simple_read(
        this->pa_handle,
        data.data(), data.size() * sizeof(float),
        &error
    );
    if (result < 0)
        throw PulseAudioException("pa_simple_read() failed", error);
}

void SimpleRecordClient::flush()
{
    int error;
    int result = pa_simple_flush(this->pa_handle, &error);
    if (result < 0)
        throw PulseAudioException("pa_simple_flush() failed", error);
}
