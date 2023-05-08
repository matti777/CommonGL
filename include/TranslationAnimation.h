#ifndef TRANSLATIONANIMATION_H
#define TRANSLATIONANIMATION_H

#include "BaseAnimation.h"

/**
 * Provides time-based, linearly interpolated animation for translating a
 * 3D point from an initial location to another.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class TranslationAnimation : public BaseAnimation
{
public: // Construction and destruction
    /**
     * Constructs the animation.
     *
     * @param initialLocation initial location is read from this adress
     * when the animation starts. Must point to float[3].
     * @param location where the result is written - must point to float[3]
     * or bigger area
     */
    TranslationAnimation(const float* initialLocation,
                         float destinationX, float destinationY,
                         float destinationZ,
                         float initialDelay, float duration, float* location);

    /**
     * Constructs the animation.
     *
     * @param location where the result is written - must point to float[3]
     * or bigger area
     */
    TranslationAnimation(float initialX, float initialY, float initialZ,
                         float destinationX, float destinationY,
                         float destinationZ,
                         float initialDelay, float duration, float* location);

    /**
     * Constructs the animation with 0,0,0 for both locations.
     *
     * @param location where the result is written - must point to float[3]
     * or bigger area
     */
    TranslationAnimation(float initialDelay, float duration, float* location);
    virtual ~TranslationAnimation();

public: // Public API
    /**
     * Animates the location, using the Time passed as parameter as the moment
     * of time. If initial delay has not passed, position is not updated.
     *
     * @return true if animation duration has expired and animation is complete
     */
    bool Animate(const TimeSample& time);

protected:
    float m_initialLocation[3];
    float m_destinationLocation[3];
    float* m_location;

    const float* m_initialLocationPtr;
};

#endif // TRANSLATIONANIMATION_H
