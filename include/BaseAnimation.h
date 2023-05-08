#ifndef BASEANIMATION_H
#define BASEANIMATION_H

#include "TimeSample.h"

/**
 * Base class for time-based animations.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class BaseAnimation
{
public:
    virtual ~BaseAnimation();

public: // Public API
    /**
     * Animates the value(s) based on the given time.
     *
     * @return true if the animation has completed, false otherwise
     */
    virtual bool Animate(const TimeSample& time) = 0;

    /**
     * Indicates whether this animation has completed, using the given
     * time as the 'current time' to compare against.
     */
    virtual bool HasCompleted(const TimeSample& time) const;

    /**
     * Indicates whether this animation has started; meaning whether
     * the initial delay has passed or not, using the given
     * time as the 'current time' to compare against.
     */
    virtual bool HasBegun(const TimeSample& time) const;

    /**
     * Indicates whether the animation is active; that is, it has begun
     * but not completed. The given time is used as the 'current time' to
     * compare against.
     */
    virtual bool IsActive(const TimeSample& time) const;

    /**
     * Resets the base time of this animation to current time.
     */
    virtual void ResetTime();
    
    /** Updates initial delay / duration values on the fly. */
    void UpdateTimings(float initialDelay, float duration);

protected:
    BaseAnimation(float initialDelay, float duration);

protected:
    TimeSample m_baseTime;

    // Initial delay in seconds; only start after this delay
    float m_initialDelay;

    // Duration of the animation in seconds
    float m_duration;
};

#endif // BASEANIMATION_H
