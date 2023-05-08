#include "SimpleTimer.h"

SimpleTimer::SimpleTimer(float initialDelay, float interval, void* userData,
                         int timerId, bool repeat, SimpleTimerCallback callback)
    : BaseAnimation(initialDelay, interval),
      m_timerId(timerId),
      m_repeat(repeat),
      m_userData(userData),
      m_callback(callback)
{
    m_nextSignalOffset = initialDelay + interval;
}

SimpleTimer::~SimpleTimer()
{
}

bool SimpleTimer::Animate(const TimeSample& time)
{
    float timeDelta = time.ElapsedTimeSince(m_baseTime);
    if ( timeDelta >= m_nextSignalOffset )
    {
        // Timer up, signal
        m_callback(this, m_timerId);

        if ( m_repeat )
        {
            m_nextSignalOffset += m_duration;
            return false;
        }
        else
        {
            return true;
        }
    }

    return false;
}

bool SimpleTimer::HasCompleted(const TimeSample& time) const
{
    if ( m_repeat )
    {
        return false;
    }
    else
    {
        return BaseAnimation::HasCompleted(time);
    }
}

