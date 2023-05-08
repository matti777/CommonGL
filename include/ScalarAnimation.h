#ifndef SCALARANIMATION_H
#define SCALARANIMATION_H

#include "BaseAnimation.h"

/**
 * Provides time-based, linearly interpolated animation for a scalar between
 * two values.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class ScalarAnimation : public BaseAnimation
{
public:
    /**
     * Constructs the animation.
     *
     * @param value where the result is written
     */
    ScalarAnimation(float initialValue, float destinationValue,
                    float initialDelay, float duration, float* value);
    virtual ~ScalarAnimation();

public: // Public API
    /**
     * Animates the value, using the Time passed as parameter as the moment
     * of time. If initial delay has not passed, value is not updated.
     *
     * @return true if animation duration has expired and animation is complete
     */
    bool Animate(const TimeSample& time);

    /** Updates the initial / destination values on the fly. */
    void UpdateValues(float initialValue, float destinationValue);

    /** Resets the animation time, duration and endpoint values. */    
    void Reset(float initialValue, float destinationValue, float duration);
    
private: // Data
    float m_initialValue;
    float m_destinationValue;
    float* m_value;
};

#endif // SCALARANIMATION_H
