#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include <stdint.h>

/**
 * Represents a moment in time and related methods.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class TimeSample
{
public: // Constructors and destructor
    TimeSample();

public: // Public API
    /**
     * Returns the time elapsed since the base time (when this object was
     * constructed or Reset() was called.
     *
     * @return elapsed time in seconds
     */
    float ElapsedTime() const;

    /**
     * Returns the time elapsed since the base time of another Time object.
     * This object's base time is used as current time.
     *
     * @return elapsed time in seconds
     */
    float ElapsedTimeSince(const TimeSample& time) const;

    /**
     * Resets the base time to current time.
     */
    void Reset();

private: // Data
    timeval m_time;
};

#endif // TIME_H
