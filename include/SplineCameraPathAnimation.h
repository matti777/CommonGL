#ifndef SPLINECAMERAPATHANIMATION_H
#define SPLINECAMERAPATHANIMATION_H

#include <vector>

#include "BSplineAnimation.h"

/**
 * Camera path animation. The route is provided as a set of 3D points and
 * a b-spline curve is fitted on them. The camera keeps looking forward
 * along the curve.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class SplineCameraPathAnimation : public BSplineAnimation
{
public: // Construction and destruction
    /**
     * Constructs the animation.
     *
     * @param points control points of the spline curve
     * @param initialDelay delay (in seconds) before the animation starts
     * @param duration duration (in seconds) of the animation
     * @param cameraPosition float[3] where to store the camera position
     * @param cameraTarget float[3] where to store the camera look-at target
     * @param targetDistance distance (in 'meters') to push camera target ahead
     * of its location
     */
    SplineCameraPathAnimation(const std::vector<Vector3>& points,
                              float initialDelay, float duration,
                              float* cameraPosition, float* cameraTarget,
                              float targetDistance);
    virtual ~SplineCameraPathAnimation();

public: // From BaseAnimation
    bool Animate(const TimeSample& time);

private: // Data
    // Number of times Animate() has been called
    int m_numUpdates;

    // Distance (in 'meters') to push camera target ahead of its location
    float m_targetDistance;

    // Buffer to store the b-spline location calculations
    float m_position[3];

    // Previous position
    float m_prevPosition[3];

    // Pointers to the caller's data
    float* m_cameraPosition;
    float* m_cameraTarget;
};

#endif // SPLINECAMERAPATHANIMATION_H
