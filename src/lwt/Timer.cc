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

//------------------------------------------------------------------------------

#include "Timer.h"
#include "BlockedThread.h"

//------------------------------------------------------------------------------

using lwt::Timer;
using lwt::BlockedThread;

//------------------------------------------------------------------------------

namespace {

//------------------------------------------------------------------------------

/**
 * A timer that blocks the current thread on creation, and unblocks
 * the process on expiration
 */
class BlockerTimer : public lwt::Timer
{
private:
    /**
     * The thread blocked by this timer
     */
    BlockedThread thread;

public:
    /**
     * Construct the timer with the given timeout and block the
     * current process.
     */
    BlockerTimer(millis_t timeout);
    
protected:
    /**
     * Unblock the process.
     */    
    virtual bool handleTimeout();
};

//------------------------------------------------------------------------------

BlockerTimer::BlockerTimer(millis_t timeout) :
    Timer(timeout)
{
    thread.blockCurrent();
}
    
//------------------------------------------------------------------------------

bool BlockerTimer::handleTimeout()
{
    thread.unblock();
    return false;
}

//------------------------------------------------------------------------------

} /* anonymous namespace */

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

Timer::timers_t Timer::timers;

//------------------------------------------------------------------------------

bool Timer::handleTimeouts()
{
    static const millis_t tolerance = 5;

    bool hadTimeouts = false;

    millis_t t = currentTimeMillis()+tolerance;
    while(!timers.empty()) {
        timers_t::iterator i = timers.begin();        
        Timer* timer = *i;

        if (timer->timeout <= t) {
            timers.erase(i);
            hadTimeouts = true;
            if (timer->handleTimeout()) {
                timers.insert(timer);
            } else {
                delete timer;
            }
        } else {
            break;
        }
    }

    return hadTimeouts || !timers.empty();
}

//------------------------------------------------------------------------------

void Timer::sleep(millis_t ms)
{
    new BlockerTimer(currentTimeMillis() + ms);
}

//------------------------------------------------------------------------------
