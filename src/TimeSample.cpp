#include <string.h>

#include "TimeSample.h"

TimeSample::TimeSample()
{
    Reset();
}

void TimeSample::Reset()
{
    gettimeofday(&m_time, NULL);
}

float TimeSample::ElapsedTime() const
{
    // Grab the current time
    timeval now;
    gettimeofday(&now, NULL);

    float secs = (now.tv_sec - m_time.tv_sec);
    float usecs = (now.tv_usec - m_time.tv_usec);

    return (secs + (usecs / 1000000));
}

float TimeSample::ElapsedTimeSince(const TimeSample& time) const
{
    float secs = (m_time.tv_sec - time.m_time.tv_sec);
    float usecs = (m_time.tv_usec - time.m_time.tv_usec);

    return (secs + (usecs / 1000000));
}
