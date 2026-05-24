#pragma once
#include <cmath>

namespace render::animation::easings
{
float easeLinear(float t) {
    return t;
}

float easeInSine(float t) {
    return sinf(3.14159265f * 0.5f * t);
}

float easeOutSine(float t) {
    return 1.0f + sinf(3.14159265f * 0.5f * (--t));
}

float easeInOutSine(float t) {
    return 0.5f * (1.0f + sinf(3.14159265f * (t - 0.5f)));
}

float easeInQuad(float t) {
    return t * t;
}

float easeOutQuad(float t) {
    return t * (2.0f - t);
}

float easeInOutQuad(float t) {
    return t < 0.5f ? 2.0f * t * t : t * (4.0f - 2.0f * t) - 1.0f;
}

float easeInCubic(float t) {
    return t * t * t;
}

float easeOutCubic(float t) {
    return 1.0f + (--t) * t * t;
}

float easeInOutCubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f + (--t) * (2.0f * (--t)) * (2.0f * t);
}

float easeInQuart(float t) {
    float t2 = t * t;
    return t2 * t2;
}

float easeOutQuart(float t) {
    float t2 = (--t) * t;
    return 1.0f - t2 * t2;
}

float easeInOutQuart(float t) {
    if (t < 0.5f) {
        float t2 = t * t;
        return 8.0f * t2 * t2;
    }
    else {
        float t2 = (--t) * t;
        return 1.0f - 8.0f * t2 * t2;
    }
}

float easeInQuint(float t) {
    float t2 = t * t;
    return t * t2 * t2;
}

float easeOutQuint(float t) {
    float t2 = (--t) * t;
    return 1.0f + t * t2 * t2;
}

float easeInOutQuint(float t) {
    float t2;
    if (t < 0.5f) {
        t2 = t * t;
        return 16.0f * t * t2 * t2;
    }
    else {
        t2 = (--t) * t;
        return 1.0f + 16.0f * t * t2 * t2;
    }
}

float easeInExpo(float t) {
    return powf(2.0f, 10.0f * t) - 1.0f;
}

float easeOutExpo(float t) {
    return 1.0f - powf(2.0f, -10.0f * t);
}

float easeInOutExpo(float t) {
    if (t < 0.5f) {
        return 0.5f * powf(2.0f, 20.0f * t) - 0.5f;
    }
    else {
        return 1.5f - 0.5f * powf(2.0f, -20.0f * (t - 1.0f));
    }
}

float easeInCirc(float t) {
    return 1.0f - sqrtf(1.0f - t);
}

float easeOutCirc(float t) {
    return sqrtf(t);
}

float easeInOutCirc(float t) {
    if (t < 0.5f) {
        return (1.0f - sqrtf(1.0f - 2.0f * t)) * 0.5f;
    }
    else {
        return (1.0f + sqrtf(2.0f * t - 1.0f)) * 0.5f;
    }
}

float easeInBack(float t) {
    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.0f;
    return c3 * t * t * t - c1 * t * t;
}

float easeOutBack(float t) {
    constexpr float c1 = 1.70158f;
    constexpr float c3 = c1 + 1.0f;
    return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
}

float easeInOutBack(float t) {
    constexpr float c1 = 1.70158f;
    constexpr float c2 = c1 * 1.525f;

    if (t < 0.5f) {
        return (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) * 0.5f;
    }
    else {
        return (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) * 0.5f;
    }
}

float easeInElastic(float t) {
    constexpr float c4 = (2.0f * 3.14159265f) / 3.0f;
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
}

float easeOutElastic(float t) {
    constexpr float c4 = (2.0f * 3.14159265f) / 3.0f;
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
}

float easeInOutElastic(float t) {
    constexpr float c5 = (2.0f * 3.14159265f) / 4.5f;

    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    if (t < 0.45f) {
        return 0.045f * powf(2.0f, 20.0f * t - 20.0f) * sinf((20.0f * t - 11.125f) * c5);
    }
    if (t < 0.55f) {
        return 0.5f + 0.5f * sinf(4.0f * 3.14159265f * t);
    }
    return 0.955f + 0.045f * powf(2.0f, 20.0f * (t - 1.0f)) * sinf((2.0f * 3.14159265f * (t + 0.5f) - 1.0f) * c5);
}

float easeInBounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    }
    else if (t < 2.0f / 2.75f) {
        return 7.5625f * (t -= 1.5f / 2.75f) * t + 0.75f;
    }
    else if (t < 2.5f / 2.75f) {
        return 7.5625f * (t -= 2.25f / 2.75f) * t + 0.9375f;
    }
    else {
        return 7.5625f * (t -= 2.625f / 2.75f) * t + 0.984375f;
    }
}

float easeOutBounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    }
    else if (t < 2.0f / 2.75f) {
        return 7.5625f * (t -= 1.5f / 2.75f) * t + 0.75f;
    }
    else if (t < 2.5f / 2.75f) {
        return 7.5625f * (t -= 2.25f / 2.75f) * t + 0.9375f;
    }
    else {
        return 7.5625f * (t -= 2.625f / 2.75f) * t + 0.984375f;
    }
}

float easeInOutBounce(float t) {
    if (t < 0.5f) {
        return easeInBounce(t * 2.0f) * 0.5f;
    }
    else {
        return easeOutBounce(t * 2.0f - 1.0f) * 0.5f + 0.5f;
    }
}
}