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

#include "PolledFD.h"

#include "Thread.h"

// #include "Log.h"

#include <cstdio>
#include <cassert>

//------------------------------------------------------------------------------

using lwt::PolledFD;

//------------------------------------------------------------------------------

PolledFD::instances_t PolledFD::instances;

//------------------------------------------------------------------------------

int PolledFD::updateAllEvents(uint32_t& events)
{
    size_t count = 0;
    for(instances_t::const_iterator i = instances.begin();
        i!=instances.end(); ++i, ++count)
    {
        PolledFD* polledFD = *i;
        int result = polledFD->updateEvents(events);

        if (result<0) {
            // Log::error("Failed to update events for FD %d\n", polledFD->fd);
            return result;
        }
    }
    // printf("PolledFD::updateAllEvents: count=%zu\n", count);
    return 0;
}

//------------------------------------------------------------------------------

int PolledFD::updateEvents(uint32_t& events)
{
    int result = 0;
    if (requestedEvents!=currentEvents) {
        if (currentEvents==0) {
            result = EPoll::get().add(this, requestedEvents);
        } else if (requestedEvents==0) {
            result = EPoll::get().remove(this);
        } else {
            result = EPoll::get().modify(this, requestedEvents);
        }
        
        if (result==0) currentEvents = requestedEvents;
    }

    events |= currentEvents;
    
    return result;
}

//------------------------------------------------------------------------------

void PolledFD::clearFD()
{
    EPoll::get().remove(this);
    ::close(fd);
    fd = -1;
}

//------------------------------------------------------------------------------

void PolledFD::setFD(int newFD, uint32_t events)
{
    if (newFD>=0) {
        fd = newFD;
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);
        requestedEvents = events;
        currentEvents = 0;
    } else {
        requestedEvents = 0;
    }
}

//------------------------------------------------------------------------------
