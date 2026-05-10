#include <ctime>
#include <cmath>
#include <algorithm>
#include <chrono>

#include "animation.h"
#include "easings.h"

namespace render
{
float getAnimatedProgress(AnimationWay way, float t)
{
    switch (way)
    {
    case ANIMATION_LINEAR:           return t;
    case ANIMATION_EASE_IN_SINE:     return animation::easings::easeInSine(t);
    case ANIMATION_EASE_OUT_SINE:    return animation::easings::easeOutSine(t);
    case ANIMATION_EASE_IN_OUT_SINE: return animation::easings::easeInOutSine(t);
    case ANIMATION_EASE_IN_QUAD:     return animation::easings::easeInQuad(t);
    case ANIMATION_EASE_OUT_QUAD:    return animation::easings::easeOutQuad(t);
    case ANIMATION_EASE_IN_OUT_QUAD: return animation::easings::easeInOutQuad(t);
    case ANIMATION_EASE_IN_CUBIC:    return animation::easings::easeInCubic(t);
    case ANIMATION_EASE_OUT_CUBIC:   return animation::easings::easeOutCubic(t);
    case ANIMATION_EASE_IN_OUT_CUBIC:return animation::easings::easeInOutCubic(t);
    case ANIMATION_EASE_IN_QUART:    return animation::easings::easeInQuart(t);
    case ANIMATION_EASE_OUT_QUART:   return animation::easings::easeOutQuart(t);
    case ANIMATION_EASE_IN_OUT_QUART:return animation::easings::easeInOutQuart(t);
    case ANIMATION_EASE_IN_QUINT:    return animation::easings::easeInQuint(t);
    case ANIMATION_EASE_OUT_QUINT:   return animation::easings::easeOutQuint(t);
    case ANIMATION_EASE_IN_OUT_QUINT:return animation::easings::easeInOutQuint(t);
    case ANIMATION_EASE_IN_EXPO:     return animation::easings::easeInExpo(t);
    case ANIMATION_EASE_OUT_EXPO:    return animation::easings::easeOutExpo(t);
    case ANIMATION_EASE_IN_OUT_EXPO: return animation::easings::easeInOutExpo(t);
    case ANIMATION_EASE_IN_CIRC:     return animation::easings::easeInCirc(t);
    case ANIMATION_EASE_OUT_CIRC:    return animation::easings::easeOutCirc(t);
    case ANIMATION_EASE_IN_OUT_CIRC: return animation::easings::easeInOutCirc(t);
    case ANIMATION_EASE_IN_BACK:     return animation::easings::easeInBack(t);
    case ANIMATION_EASE_OUT_BACK:    return animation::easings::easeOutBack(t);
    case ANIMATION_EASE_IN_OUT_BACK: return animation::easings::easeInOutBack(t);
    case ANIMATION_EASE_IN_ELASTIC:  return animation::easings::easeInElastic(t);
    case ANIMATION_EASE_OUT_ELASTIC: return animation::easings::easeOutElastic(t);
    case ANIMATION_EASE_IN_OUT_ELASTIC: return animation::easings::easeInOutElastic(t);
    case ANIMATION_EASE_IN_BOUNCE: return animation::easings::easeInBounce(t);
    case ANIMATION_EASE_OUT_BOUNCE:  return animation::easings::easeOutBounce(t);
    case ANIMATION_EASE_IN_OUT_BOUNCE:return animation::easings::easeInOutBounce(t);

    default: return t;
    }
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
    auto now = std::chrono::high_resolution_clock::now();

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

    std::chrono::duration<float> elapsed = now - timeOnTrigger;
    animationProgress = elapsed.count() / duration;
    animationProgress = std::clamp(animationProgress, 0.f, 1.f);

    float progress = getAnimatedProgress(animationWay, animationProgress);
    animatedValue = std::lerp(begin, condition ? end : start, progress);
}

float Animation::getAnimatedValue()
{
    return animatedValue;
}
}