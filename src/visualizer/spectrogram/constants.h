#ifndef CONSTANTS_H
#define CONSTANTS_H

const struct
{
    float fft_size = 2048;
    float window_size = 1024;
    float window_overlap = 0.5;
    float y_min = -70.0;
    float y_max = 0.0;
    bool scrolling = true;
} constants;

#endif // CONSTANTS_H
