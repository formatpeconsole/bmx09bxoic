#pragma once
#include <chrono>

namespace render
{
enum AnimationWay
{
    ANIMATION_LINEAR = 0,
    ANIMATION_EASE_IN_SINE,
    ANIMATION_EASE_OUT_SINE,
    ANIMATION_EASE_IN_OUT_SINE,
    ANIMATION_EASE_IN_QUAD,
    ANIMATION_EASE_OUT_QUAD,
    ANIMATION_EASE_IN_OUT_QUAD,
    ANIMATION_EASE_IN_CUBIC,
    ANIMATION_EASE_OUT_CUBIC,
    ANIMATION_EASE_IN_OUT_CUBIC,
    ANIMATION_EASE_IN_QUART,
    ANIMATION_EASE_OUT_QUART,
    ANIMATION_EASE_IN_OUT_QUART,
    ANIMATION_EASE_IN_QUINT,
    ANIMATION_EASE_OUT_QUINT,
    ANIMATION_EASE_IN_OUT_QUINT,
    ANIMATION_EASE_IN_EXPO,
    ANIMATION_EASE_OUT_EXPO,
    ANIMATION_EASE_IN_OUT_EXPO,
    ANIMATION_EASE_IN_CIRC,
    ANIMATION_EASE_OUT_CIRC,
    ANIMATION_EASE_IN_OUT_CIRC,
    ANIMATION_EASE_IN_BACK,
    ANIMATION_EASE_OUT_BACK,
    ANIMATION_EASE_IN_OUT_BACK,
    ANIMATION_EASE_IN_ELASTIC,
    ANIMATION_EASE_OUT_ELASTIC,
    ANIMATION_EASE_IN_OUT_ELASTIC,
    ANIMATION_EASE_IN_BOUNCE,
    ANIMATION_EASE_OUT_BOUNCE,
    ANIMATION_EASE_IN_OUT_BOUNCE
};

enum AnimationFlags : uint8_t
{
    ANIMATION_FLAGS_NONE = 0,
    ANIMATION_FLAGS_REPLAY_FROM_START = 1 << 0,
    ANIMATION_FLAGS_REPLAY_FROM_END = 1 << 1,
};

class Animation
{
private:
    std::chrono::steady_clock::time_point timeOnTrigger{};
    float duration{};
    float start{};
    float begin{};
    float end{};
    float animationProgress{};
    float animatedValue{};

    AnimationWay animationWay = ANIMATION_LINEAR;
    uint8_t animationFlags = ANIMATION_FLAGS_NONE;

    bool condition{};
    bool previousCondition{};
public:
    Animation(float start, float end, float duration, AnimationWay way, uint8_t flags = ANIMATION_FLAGS_NONE);

    void setCondition(bool triggerCondition);
    void process();

    float getAnimatedValue();
};
}