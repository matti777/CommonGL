#ifndef FPSMETER_H
#define FPSMETER_H

#include <sys/time.h>
#include <stdint.h>

// number of latest frames to include in the calculation
static const int SampleBufferSize = 50;

/**
 * Provides accurate frames-per-second ("FPS") calculation. The implementation
 * is based on an efficient circular buffer of the latest N samples.
 */
class FpsMeter
{
public:
    FpsMeter();

public:
    /** Marks a new frame starting. */
    void StartFrame();

    /**
     * Ends the current frame (if any). A new sample is added. It is safe
     * to call this method without calling StartFrame() first; in that case
     * this method simply does nothing.
     */
    void EndFrame();

    /** Resets the FPS calculations, clearing all data. */
    void Reset();

    /** Returns the current FPS */
    float GetFps() const;

private:
    int m_nextSampleIndex;
    timeval m_frameStartTime;

    // Whether there is a frame being measured
    bool m_hasActiveFrame;

    // measured samples in microseconds. a sample indicates how long it took
    // to render a frame
    uint32_t m_sampleBuffer[SampleBufferSize];
};

#endif // FPSMETER_H
