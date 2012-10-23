//
// Copyright (c) 2011 by István Váradi
//
// This file is part of proxymux, a web proxy multiplexing utility

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef LWT_TIMER_H
#define LWT_TIMER_H
//------------------------------------------------------------------------------

#include "util.h"

#include <set>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * Timer
 */
class Timer
{
private:
    /**
     * Less class for the timer
     */
    struct Less
    {
        bool operator()(const Timer* timer1, const Timer* timer2) const;
    };

    /**
     * Type for the set of timers
     */
    typedef std::multiset<Timer*, Less> timers_t;

    /**
     * The set of timers
     */
    static timers_t timers;

public:
    /**
     * Get the earliest timeout or INVALID_MILLIS, if there is no
     * timer
     */
    static millis_t getEarliest();

    /**
     * Handle the timeouts. 
     *
     * @return if there were any timeouts handled or if there are any
     * pending ones.
     */
    static bool handleTimeouts();

    /**
     * Sleep the given number of milliseconds.
     */
    static void sleep(millis_t ms);
    
protected:
    /**
     * The timeout
     */
    millis_t timeout;

public:
    /**
     * Construct the timer with the given timeout. It will be added to
     * the set of timers.
     */
    Timer(millis_t timeout);

    /**
     * Destroy the timer. It will not be removed from the set, it
     * should be removed separately.
     */
    virtual ~Timer();

    /**
     * Cancel this timer. It will be removed from the set.
     */
    void cancel();

protected:
    /**
     * Handle the timeout. The timer should be removed before this
     * function is called.
     *
     * @return if the timer should be reused. If so, it will be put
     * back to the set of timers, with a (hopefully) new timeout set
     * by this function. Otherwise it will be deleted.
     */
    virtual bool handleTimeout() = 0;
};

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline bool Timer::Less::operator()(const Timer* timer1, const Timer* timer2) const
{
    return timer1->timeout < timer2->timeout;
}

//------------------------------------------------------------------------------

inline millis_t Timer::getEarliest()
{
    return timers.empty() ? INVALID_MILLIS : (*timers.begin())->timeout;
}

//------------------------------------------------------------------------------

inline Timer::Timer(millis_t timeout) :
    timeout(timeout)
{
    timers.insert(this);
}

//------------------------------------------------------------------------------

inline Timer::~Timer()
{
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // PROXYMUX_TIMER_H

// Local variables:
// mode: c++
// End:
