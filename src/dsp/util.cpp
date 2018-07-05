#define _USE_MATH_DEFINES
#include <math.h>
#include "util.h"

float convertToDecibel(float magnitude, float dB_min)
{
    // Convert the magnitude into dbFS and clamp it between [-min_dB; 0]
    if (magnitude == 0.0f)
        magnitude = dB_min;
    else
    {
        magnitude = 20.0f * log10(magnitude);
        if (magnitude < dB_min)
            magnitude = dB_min;
    }

    // TODO: Remove this
    if (magnitude > 0.0f)
        throw "This should not happen at all";

    return magnitude;
}
