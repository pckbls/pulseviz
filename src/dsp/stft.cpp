#define _USE_MATH_DEFINES
#include <math.h>
#include "stft.h"

// TODO: Use templates!
std::vector<float> generateHammingWindow(unsigned int n)
{
    std::vector<float> result(n);
    for (unsigned int i = 0; i < n; i++)
        result[i] = 25.0/46.0 - 21.0/46.0 * cos(2 * M_PI * i / (n - 1));
    return result;
}

// TODO: Rename fft_size to sample_size?
// TODO: Add window_size parameter
STFT::STFT(SimpleRecordClient& src, size_t fft_size, size_t window_size, float window_overlap, STFTWindow window)
    :
    coefficients(fft_size / 2 + 1),
    frequencies(fft_size / 2 + 1),
    sampler(
        src,
        fft_size,
        std::floor((float)window_size * (1.0 - window_overlap))
    )
{
    this->in = fftwf_alloc_real(fft_size);
    this->out = fftwf_alloc_complex(fft_size); // TODO: Can be smaller!
    this->plan = fftwf_plan_dft_r2c_1d(fft_size, this->in, this->out, FFTW_ESTIMATE);

    switch (window)
    {
        case STFTWindow::HAMMING: this->window = generateHammingWindow(fft_size); break;
        default: throw "Unknown window type"; break;
    }

    this->window_sum = 0.0;
    for (float& value: this->window)
        this->window_sum += value;

    for (unsigned int i = 0; i < this->coefficients.size(); i++)
    {
        // TODO: This is probably wrong.
        this->frequencies[i] = i / src.getSampleRate();
    }
}

STFT::~STFT()
{
    fftwf_destroy_plan(plan);
    fftwf_free(this->in);
    fftwf_free(this->out);
}

void STFT::slide()
{
    this->sampler.readChunk();
    this->sampler.appendToBuffer();

    for (unsigned int i = 0; i < this->sampler.data.size(); i++)
        this->in[i] = this->sampler.data[i] * this->window[i];

    fftwf_execute(plan);

    for (size_t i = 0; i < this->coefficients.size(); i++)
    {
        // TODO: This might be wrong!
        // Interesting side note: this->spectrum.data()[i] is SOOOOO much slower than this->spectrum.data
        float abs_value = sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);

        abs_value *= 2.0;
        abs_value *= abs_value;
        abs_value /= pow(this->window_sum, 2.0);

        if (abs_value == 0.0)
            abs_value = -200.0; // TODO
        else
            abs_value = 10.0 * log10(abs_value);

        // TODO !!!
        if (abs_value < -200)
            abs_value = -200.0;

        this->coefficients[i] = abs_value;
    }
}

const std::vector<float>& STFT::getFrequencies()
{
    return this->frequencies;
}
