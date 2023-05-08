#ifndef BSPLINEANIMATION_H
#define BSPLINEANIMATION_H

#include <vector>

#include "OpenGLAPI.h"
#include "BaseAnimation.h"

/**
 * Provides a smooth curve fitting cubic b-spline animation.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class BSplineAnimation : public BaseAnimation
{
public: // Construction and destruction
    /**
     * Constructs the animation.
     *
     * @param points control points of the spline curve
     * @param initialDelay delay (in seconds) before the animation starts
     * @param duration duration (in seconds) of the animation
     * @param value float[3] where to store the result
     */
    BSplineAnimation(const std::vector<Vector3>& points,
                     float initialDelay, float duration, float* value);
    virtual ~BSplineAnimation();

public: // From BaseAnimation
    bool Animate(const TimeSample& time);

private:
    float Interpolate(float p0, float p1, float p2, float p3,
                      float t, float tt, float ttt);
    void Calculate(float t);

protected: // Data
    // Control points of the spline curve
    std::vector<Vector3> m_points;

    // Where to store the calculation result (interpolated location on the
    // spline curve). Not owned.
    float* m_value;
};

#endif // BSPLINEANIMATION_H
