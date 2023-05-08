#include <stdio.h>

#include "ScalarAnimation.h"
#include "CommonFunctions.h"

ScalarAnimation::ScalarAnimation(float initialValue, float destinationValue,
                                 float initialDelay, float duration,
                                 float* value)
    : BaseAnimation(initialDelay, duration),
      m_initialValue(initialValue),
      m_destinationValue(destinationValue),
      m_value(value)
{
}

ScalarAnimation::~ScalarAnimation()
{
    // Not owned
    m_value = NULL;
}

bool ScalarAnimation::Animate(const TimeSample& time)
{
    float elapsed = time.ElapsedTimeSince(m_baseTime);
    bool completed = false;
    if ( elapsed > (m_initialDelay + m_duration) )
    {
        // Animation duration expired; animation has completed
        *m_value = m_destinationValue;
        completed = true;
    }
    else if ( elapsed >= m_initialDelay )
    {
        // Ongoing; interpolate position based on the time value
        float factor = (elapsed - m_initialDelay) / m_duration;
        float dvalue = m_destinationValue - m_initialValue;
        *m_value = m_initialValue + (dvalue * factor);
    }
    
    return completed;
}

void ScalarAnimation::UpdateValues(float initialValue, float destinationValue)
{
    m_initialValue = initialValue;
    m_destinationValue = destinationValue;
}

void ScalarAnimation::Reset(float initialValue, float destinationValue, 
                            float duration)
{
    m_baseTime.Reset();
    m_initialValue = initialValue;
    m_destinationValue = destinationValue;
    m_duration = duration;
}

