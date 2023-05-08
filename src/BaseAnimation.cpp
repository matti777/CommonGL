#include "BaseAnimation.h"

BaseAnimation::BaseAnimation(float initialDelay, float duration)
    : m_baseTime(),
    m_initialDelay(initialDelay),
      m_duration(duration)
{
}

BaseAnimation::~BaseAnimation()
{
}

bool BaseAnimation::HasCompleted(const TimeSample& time) const
{
    float elapsed = time.ElapsedTimeSince(m_baseTime);
    return ( elapsed > (m_initialDelay + m_duration) );
}

bool BaseAnimation::HasBegun(const TimeSample& time) const
{
    float elapsed = time.ElapsedTimeSince(m_baseTime);
    return ( elapsed >= m_initialDelay );
}

bool BaseAnimation::IsActive(const TimeSample& time) const
{
    return ( HasBegun(time) && !HasCompleted(time) );
}

void BaseAnimation::ResetTime()
{
    m_baseTime.Reset();
}

void BaseAnimation::UpdateTimings(float initialDelay, float duration)
{
    m_initialDelay = initialDelay;
    m_duration = duration;
}
