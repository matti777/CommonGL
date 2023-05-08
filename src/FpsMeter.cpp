#include <memory.h>
#include <string.h>

#include "FpsMeter.h"

FpsMeter::FpsMeter()
    : m_nextSampleIndex(0),
      m_hasActiveFrame(false)
{
    memset(&m_frameStartTime, 0, sizeof(m_frameStartTime));
    memset(m_sampleBuffer, 0, sizeof(m_sampleBuffer));
}

void FpsMeter::StartFrame()
{
    gettimeofday(&m_frameStartTime, NULL);
    m_hasActiveFrame = true;
}

void FpsMeter::EndFrame()
{
    if ( !m_hasActiveFrame )
    {
        return;
    }

    timeval now;
    gettimeofday(&now, NULL);

    // calculate sample duration in milliseconds
    uint32_t sampleTime = (now.tv_sec - m_frameStartTime.tv_sec) * 1000000;
    sampleTime += (now.tv_usec - m_frameStartTime.tv_usec);

    // put the sample into the sample circular buffer at the correct location
    m_sampleBuffer[m_nextSampleIndex++] = sampleTime;
    if ( m_nextSampleIndex >= SampleBufferSize )
    {
        m_nextSampleIndex = 0;
    }

    m_hasActiveFrame = false;
}

void FpsMeter::Reset()
{
    memset(&m_frameStartTime, 0, sizeof(m_frameStartTime));
    memset(m_sampleBuffer, 0, sizeof(m_sampleBuffer));
    m_nextSampleIndex = 0;
    m_hasActiveFrame = false;
}

float FpsMeter::GetFps() const
{
    // calculate the average render time as SUM(all samples) / num_samples
    int numSamples = 0;
    uint32_t sum = 0;
    for ( int i = 0; i < SampleBufferSize; i++ )
    {
        uint32_t sample = m_sampleBuffer[i];
        if ( sample > 0 )
        {
            numSamples++;
            sum += sample;
        }
    }

    float avgRenderTime = float(sum) / float(numSamples);

    // FPS = time / avgRenderTime (usec)
    return 1000000.0 / avgRenderTime;
}
