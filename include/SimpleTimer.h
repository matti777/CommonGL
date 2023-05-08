#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include "BaseAnimation.h"

// Forward declarations
class SimpleTimer;

// Timer callback
typedef void (*SimpleTimerCallback)(SimpleTimer* timer, int timerId);

/**
 * Simple timer built on the BaseAnimation framework. Emits a signal (calls
 * a callback) after the duration is up. Can be set up to repeat the signal
 * or to be one time thing.
 *
 * @author Matti Dahlbom
 * @since 0.1
 */
class SimpleTimer : public BaseAnimation
{
public: // Construction and destruction
    /**
     * Constructs the timer.
     *
     * @param initialDelay time in seconds to wait before starting the cycle
     * @param interval time in seconds before the first signal, or time in
     * seconds between the signals (repeat mode)
     * @param userData free form user data
     * @param timerId caller-supplied id for the timer. can be used to connect
     * callbacks to timers.
     * @param repeat repeat mode on (true) or off (false)
     * @param callback pointer to the callback function
     */
    SimpleTimer(float initialDelay, float interval, void* userData,
                int timerId, bool repeat, SimpleTimerCallback callback);
    virtual ~SimpleTimer();

public: // Public API
    void* GetUserData() const { return m_userData; }
    bool GetRepeat() const { return m_repeat; }

public: // From BaseAnimation
    virtual bool Animate(const TimeSample& time);
    virtual bool HasCompleted(const TimeSample& time) const;

private: // Data
    int m_timerId;
    bool m_repeat;
    void* m_userData;
    SimpleTimerCallback m_callback;

    // Time offset (in seconds) to the next signal from the anim base time
    float m_nextSignalOffset;
};

#endif // SIMPLETIMER_H
