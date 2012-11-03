//
// Copyright (c) 2011 by István Váradi
//
// This file is part of liblwt, a Lightweight (Cooperative) Threading library

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

#include "Scheduler.h"
#include "Timer.h"

#include <cstdio>

//------------------------------------------------------------------------------

using lwt::Scheduler;

//------------------------------------------------------------------------------

Scheduler* Scheduler::instance = 0;

//------------------------------------------------------------------------------

void Scheduler::run()
{
    bool hadEvents = true;
    while(hadEvents) {
        if (saveContext(context)==0) {
            processReady();
        }

        millis_t earliest = Timer::getEarliest();
        int timeout = -1;
        if (earliest!=INVALID_MILLIS) {
            millis_t now = currentTimeMillis();
            if (earliest<=now) timeout = 0;
            else timeout = earliest - now;
        }

        int result = epoll.wait(hadEvents, timeout);
        if (result<0) {
            assert(0 && "epoll failed");
        }
        // printf("Scheduler::run0: hadEvents=%d\n", hadEvents);
        hadEvents = Timer::handleTimeouts() || hadEvents;
        // printf("Scheduler::run1: hadEvents=%d\n", hadEvents);
    }
}

//------------------------------------------------------------------------------

void Scheduler::processReady()
{
    if (readyFirst==0) return;

    Thread* thread = readyFirst;
    thread->remove(readyFirst, readyLast);
    Thread::current = thread;
    
    thread->resume();
}

//------------------------------------------------------------------------------
