#include <math.h>

#include "BSplineAnimation.h"
#include "CommonFunctions.h"

BSplineAnimation::BSplineAnimation(const std::vector<Vector3>& points,
                                   float initialDelay, float duration,
                                   float* value)
    : BaseAnimation(initialDelay, duration),
      m_points(points),
      m_value(value)
{
}

BSplineAnimation::~BSplineAnimation()
{
    m_points.clear();

    // Not owned
    m_value = NULL;
}

float BSplineAnimation::Interpolate(float p0, float p1, float p2, float p3,
                                    float t, float tt, float ttt)
{
    float v0 = (p2 - p0) * 0.5;
    float v1 = (p3 - p1) * 0.5;
    return ((2 * (p1 - p2) + v0 + v1) * ttt) +
            ((-3 * (p1 - p2) - (2 * v0) - v1) * tt) +
            ((v0 * t) + p1);
}

void BSplineAnimation::Calculate(float t)
{
    // This code has been borrowed from THREE.js Spline class
    int numPoints = m_points.size();
    float tmul = (numPoints - 1) * t;
    int pointIndex = (int)floor(tmul);
    float weight = tmul - floor(tmul);

    // Figure out control point indices to use for calculation
    int i0 = (pointIndex == 0) ? pointIndex : (pointIndex - 1);
    int i1 = pointIndex;
    int i2 = (pointIndex > (numPoints - 2)) ? pointIndex : pointIndex + 1;
    int i3 = (pointIndex > (numPoints - 3)) ? pointIndex : pointIndex + 2;

    // Grab the control points indicated by the indices
    const Vector3& cp0 = m_points[i0];
    const Vector3& cp1 = m_points[i1];
    const Vector3& cp2 = m_points[i2];
    const Vector3& cp3 = m_points[i3];

    // Calculate weight^2 and weight^3 (weight = fractional part of t)
    float w2 = weight * weight;
    float w3 = w2 * weight;

    // Calculate the coordinate using cubic b-spline interpolation
    m_value[0] = Interpolate(cp0.x, cp1.x, cp2.x, cp3.x, weight, w2, w3);
    m_value[1] = Interpolate(cp0.y, cp1.y, cp2.y, cp3.y, weight, w2, w3);
    m_value[2] = Interpolate(cp0.z, cp1.z, cp2.z, cp3.z, weight, w2, w3);
}

bool BSplineAnimation::Animate(const TimeSample& time)
{
    float elapsed = time.ElapsedTimeSince(m_baseTime);
    bool completed = false;

    if ( elapsed >= (m_initialDelay + m_duration) )
    {
        // Animation duration expired; animation has completed
        const Vector3& destination = m_points.back();
        m_value[0] = destination.x;
        m_value[1] = destination.y;
        m_value[2] = destination.z;
        completed = true;
    }
    else if ( elapsed >= m_initialDelay )
    {
        // Ongoing; interpolate position based on the time value
        float t = (elapsed - m_initialDelay) / m_duration;
        Calculate(t);
    }

    return completed;
}

