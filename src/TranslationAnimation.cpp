#include <string.h>

#include "TranslationAnimation.h"
#include "MatrixOperations.h"
#include "CommonFunctions.h"

TranslationAnimation::TranslationAnimation(const float* initialLocation,
                                           float destinationX,
                                           float destinationY,
                                           float destinationZ,
                                           float initialDelay,
                                           float duration, float* location)
    : BaseAnimation(initialDelay, duration),
      m_location(location),
      m_initialLocationPtr(initialLocation)
{
    m_destinationLocation[0] = destinationX;
    m_destinationLocation[1] = destinationY;
    m_destinationLocation[2] = destinationZ;
}

TranslationAnimation::TranslationAnimation(float initialX, float initialY,
                                           float initialZ,
                                           float destinationX,
                                           float destinationY,
                                           float destinationZ,
                                           float initialDelay, float duration,
                                           float* location)
    : BaseAnimation(initialDelay, duration),
      m_location(location),
      m_initialLocationPtr(NULL)
{
    m_initialLocation[0] = initialX;
    m_initialLocation[1] = initialY;
    m_initialLocation[2] = initialZ;
    m_destinationLocation[0] = destinationX;
    m_destinationLocation[1] = destinationY;
    m_destinationLocation[2] = destinationZ;
}

TranslationAnimation::TranslationAnimation(float initialDelay, float duration,
                                           float* location)
    : BaseAnimation(initialDelay, duration),
      m_location(location)
{
    memset(&m_initialLocation, 0, sizeof(m_initialLocation));
    memset(&m_destinationLocation, 0, sizeof(m_destinationLocation));
}

TranslationAnimation::~TranslationAnimation()
{
    // Not owned resources
    m_location = NULL;
    m_initialLocationPtr = NULL;
}

bool TranslationAnimation::Animate(const TimeSample& time)
{
    float elapsed = time.ElapsedTimeSince(m_baseTime);
    bool completed = false;

    if ( elapsed >= (m_initialDelay + m_duration) )
    {
        // Animation duration expired; animation has completed
        CopyVector(m_destinationLocation, m_location);
        completed = true;
    }
    else if ( elapsed >= m_initialDelay )
    {
        // Ongoing; on first iteration, read the initial location from the
        // supplied address if pointer supplied
        if ( m_initialLocationPtr != NULL )
        {
            m_initialLocation[0] = m_initialLocationPtr[0];
            m_initialLocation[1] = m_initialLocationPtr[1];
            m_initialLocation[2] = m_initialLocationPtr[2];
            m_initialLocationPtr = NULL;
        }

        // Interpolate position based on the time value
        float factor = (elapsed - m_initialDelay) / m_duration;
        float dx = m_destinationLocation[0] - m_initialLocation[0];
        float dy = m_destinationLocation[1] - m_initialLocation[1];
        float dz = m_destinationLocation[2] - m_initialLocation[2];
        m_location[0] = m_initialLocation[0] + (dx * factor);
        m_location[1] = m_initialLocation[1] + (dy * factor);
        m_location[2] = m_initialLocation[2] + (dz * factor);
    }

    return completed;
}
