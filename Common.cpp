#include "common.h"

const float Pi = 3.14159265f;

// Get the maximum between two values.
float maximum(float a, float b)
{
    return (a > b ? a : b);
}

// Clamp a value between a minimum and maximum.
float clamp(float value, float minimum, float maximum)
{
    if (value < minimum) {
        return minimum;
    }
    else if (value > maximum) {
        return maximum;
    }
    return value;
}

// Linear interpolate between two values.
float lerp(float a, float b, float blend)
{
    return a + ((b - a) * blend);
}
