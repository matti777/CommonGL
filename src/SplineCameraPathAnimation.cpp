#include <math.h>

#include "SplineCameraPathAnimation.h"
#include "CommonFunctions.h"

SplineCameraPathAnimation::SplineCameraPathAnimation(
        const std::vector<Vector3>& points,
        float initialDelay, float duration,
        float* cameraPosition, float* cameraTarget, float targetDistance)
    : BSplineAnimation(points, initialDelay, duration, m_position),
      m_numUpdates(0),
      m_targetDistance(targetDistance),
      m_cameraPosition(cameraPosition),
      m_cameraTarget(cameraTarget)
{
}

SplineCameraPathAnimation::~SplineCameraPathAnimation()
{
    LOG_DEBUG("SplineCameraPathAnimation::~SplineCameraPathAnimation()");

    // Not owned    
    m_cameraPosition = NULL;
    m_cameraTarget = NULL;
}

bool SplineCameraPathAnimation::Animate(const TimeSample& time)
{
    if ( time.ElapsedTimeSince(m_baseTime) < m_initialDelay )
    {
        return false;
    }

    // Call BSplineAnimation to do the real work..
    bool done = BSplineAnimation::Animate(time);

    if ( !done )
    {
        if ( m_numUpdates > 0 )
        {
            // Set camera position to previous camera target
            m_cameraPosition[0] = m_prevPosition[0];
            m_cameraPosition[1] = m_prevPosition[1];
            m_cameraPosition[2] = m_prevPosition[2];
        }
        else
        {
            // First call to Animate(); set camera position to first point
            const Vector3& point = m_points.front();
            m_cameraPosition[0] = point.x;
            m_cameraPosition[1] = point.y;
            m_cameraPosition[2] = point.z;
        }

        // Save current position for next iteration to be used for
        // the m_cameraPosition
        float x = m_position[0];
        float y = m_position[1];
        float z = m_position[2];
        m_prevPosition[0] = x;
        m_prevPosition[1] = y;
        m_prevPosition[2] = z;

        // Calculate camera target ahead of the location
        float dx = x - m_cameraPosition[0];
        float dy = y - m_cameraPosition[1];
        float dz = z - m_cameraPosition[2];
        float len = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
        float f = m_targetDistance / len;
        m_cameraTarget[0] = m_cameraPosition[0] + (dx * f);
        m_cameraTarget[1] = m_cameraPosition[1] + (dy * f);
        m_cameraTarget[2] = m_cameraPosition[2] + (dz * f);

        m_numUpdates++;
    }

    return done;
}

