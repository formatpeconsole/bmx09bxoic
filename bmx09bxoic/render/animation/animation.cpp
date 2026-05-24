#include <ctime>
#include <cmath>
#include <algorithm>
#include <chrono>

#include "animation.h"
#include "easings.h"

#include "../../clock/clock.h"

namespace render
{
using EasingFunc = float(*)(float);
constexpr EasingFunc easingTable[] = {
    animation::easings::easeLinear, // ANIMATION_LINEAR
    animation::easings::easeInSine, // ANIMATION_EASE_IN_SINE
    animation::easings::easeOutSine, // ANIMATION_EASE_OUT_SINE
    animation::easings::easeInOutSine, // ANIMATION_EASE_IN_OUT_SINE
    animation::easings::easeInQuad, // ANIMATION_EASE_IN_QUAD
    animation::easings::easeOutQuad, // ANIMATION_EASE_OUT_QUAD
    animation::easings::easeInOutQuad, // ANIMATION_EASE_IN_OUT_QUAD
    animation::easings::easeInCubic, // ANIMATION_EASE_IN_CUBIC
    animation::easings::easeOutCubic, // ANIMATION_EASE_OUT_CUBIC
    animation::easings::easeInOutCubic, // ANIMATION_EASE_IN_OUT_CUBIC
    animation::easings::easeInQuart, // ANIMATION_EASE_IN_QUART
    animation::easings::easeOutQuart, // ANIMATION_EASE_OUT_QUART
    animation::easings::easeInOutQuart, // ANIMATION_EASE_IN_OUT_QUART
    animation::easings::easeInQuint, // ANIMATION_EASE_IN_QUINT
    animation::easings::easeOutQuint, // ANIMATION_EASE_OUT_QUINT
    animation::easings::easeInOutQuint, // ANIMATION_EASE_IN_OUT_QUINT
    animation::easings::easeInExpo, // ANIMATION_EASE_IN_EXPO
    animation::easings::easeOutExpo, // ANIMATION_EASE_OUT_EXPO
    animation::easings::easeInOutExpo, // ANIMATION_EASE_IN_OUT_EXPO
    animation::easings::easeInCirc, // ANIMATION_EASE_IN_CIRC
    animation::easings::easeOutCirc, // ANIMATION_EASE_OUT_CIRC
    animation::easings::easeInOutCirc, // ANIMATION_EASE_IN_OUT_CIRC
    animation::easings::easeInBack, // ANIMATION_EASE_IN_BACK
    animation::easings::easeOutBack, // ANIMATION_EASE_OUT_BACK
    animation::easings::easeInOutBack, // ANIMATION_EASE_IN_OUT_BACK
    animation::easings::easeInElastic, // ANIMATION_EASE_IN_ELASTIC
    animation::easings::easeOutElastic, // ANIMATION_EASE_OUT_ELASTIC
    animation::easings::easeInOutElastic, // ANIMATION_EASE_IN_OUT_ELASTIC
    animation::easings::easeInBounce, // ANIMATION_EASE_IN_BOUNCE
    animation::easings::easeOutBounce, // ANIMATION_EASE_OUT_BOUNCE
    animation::easings::easeInOutBounce, // ANIMATION_EASE_IN_OUT_BOUNCE
};

inline float getAnimatedProgress(AnimationWay way, float t)
{
    auto index = static_cast<std::size_t>(way);
    return easingTable[index](t);
}

Animation::Animation(float start, float end, float duration, AnimationWay way, uint8_t flags)
    : start(start), end(end), begin(0.f), animatedValue(0.f), animationProgress(1.f),
    duration(duration), animationWay(way), animationFlags(flags) {}

void Animation::setCondition(bool triggerCondition)
{
    condition = triggerCondition;
}

void Animation::process()
{
    auto now = getClockInstance().getTime();
    if (previousCondition != condition)
    {
        timeOnTrigger = now;
        animationProgress = 0.f;
        previousCondition = condition;

        if ((animationFlags & ANIMATION_FLAGS_REPLAY_FROM_START))
            animatedValue = start;
        else if ((animationFlags & ANIMATION_FLAGS_REPLAY_FROM_END))
            animatedValue = end;

        begin = animatedValue;

        return;
    }

    if (animationProgress < 1.f)
    {
        std::chrono::duration<float> elapsed = now - timeOnTrigger;
        animationProgress = elapsed.count() / duration;
        animationProgress = std::clamp(animationProgress, 0.f, 1.f);

        float progress = getAnimatedProgress(animationWay, animationProgress);
        animatedValue = std::lerp(begin, condition ? end : start, progress);
    }
    else
    {
        if (animatedValue == 0.f && begin == 0.f)
        {
            if ((animationFlags & ANIMATION_FLAGS_REPLAY_FROM_START))
                begin = end;
            else if ((animationFlags & ANIMATION_FLAGS_REPLAY_FROM_END))
                begin = start;
            else
                begin = condition ? end : start;

            animatedValue = begin;
        }
    }
}

float Animation::getAnimatedValue()
{
    return animatedValue;
}
}