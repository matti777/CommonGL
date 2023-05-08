#include "RotationAnimation.h"

RotationAnimation::RotationAnimation(float initialRotation, float rotationSpeed,
                                     float* value)
    : BaseAnimation(0.0, 0.0),
      m_rotation(initialRotation),
      m_rotationSpeed(rotationSpeed),
      m_value(value)
{
}

RotationAnimation::~RotationAnimation()
{
}

bool RotationAnimation::Animate(const TimeSample& time)
{
    // Get the time since last sample in seconds
    float timeDelta = time.ElapsedTimeSince(m_prevSampleTime);

    // Update the rotation
    m_rotation += timeDelta * m_rotationSpeed;
    *m_value = m_rotation;

    // Record the sampling time
    m_prevSampleTime = time;

    return false;
}

void RotationAnimation::SetRotationSpeed(float rotationSpeed)
{
    m_rotationSpeed = rotationSpeed;
}
