#ifndef ROTATIONANIMATION_H
#define ROTATIONANIMATION_H

#include "BaseAnimation.h"

/**
 * Provides time-based, linearly interpolated animation for rotation.
 * The rotation speed can be configured. Useful for vehicle wheels etc.
 * This animation has no duration; it goes on forever.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class RotationAnimation : public BaseAnimation
{
public:
    /**
     * Constructs the animation.
     *
     * @param initialRotation initial rotation in radians
     * @param rotationSpeed radians/second
     * @param value where the result is written
     */
    RotationAnimation(float initialRotation, float rotationSpeed, float* value);
    virtual ~RotationAnimation();

public: // Public API
    /**
     * Animates the value, using the Time passed as parameter as the moment
     * of time. If initial delay has not passed, value is not updated.
     *
     * @return true if animation duration has expired and animation is complete
     */
    bool Animate(const TimeSample& time);

    /**
     * Sets the rotation speed.
     *
     * @param rotationSpeed radians/second
     */
    void SetRotationSpeed(float rotationSpeed);

private: // Data
    float m_rotation;
    float m_rotationSpeed;
    float* m_value;

    // Previous sample time (when Animate() was last called)
    TimeSample m_prevSampleTime;
};

#endif // ROTATIONANIMATION_H
