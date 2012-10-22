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

#include "EPoll.h"
#include "PolledFD.h"
//#include "Log.h"

#include <cstring>
#include <cerrno>

//------------------------------------------------------------------------------

using lwt::EPoll;

using std::set;

//------------------------------------------------------------------------------

EPoll* EPoll::instance = 0;

//------------------------------------------------------------------------------

void EPoll::destroy(PolledFD* polledFD)
{
    if (polledFD!=0) {
        if (processing) {
            fdsToDelete.insert(polledFD);
        } else {
            delete polledFD;
        }
    }
}

//------------------------------------------------------------------------------

int EPoll::wait(bool& hadEvents, int timeout)
{
    static const size_t NUM_EVENTS=16;

    uint32_t eventMask = 0;
    PolledFD::updateAllEvents(eventMask);
    hadEvents = eventMask!=0 || timeout>=0;
    if (!hadEvents) return 0;

    struct epoll_event events[NUM_EVENTS];
    
    memset(events, 0, sizeof(events));
    errno = 0;
    int a = epoll_wait(epfd, events, NUM_EVENTS, timeout);
    // Log::debug("epoll_wait: timeout=%d, a=%d, errno=%d\n", timeout,
    //            a, errno);
    if (a>=0) {
        processing = true;
        for(int i = 0; i<a; ++i) {
            PolledFD* fd = reinterpret_cast<PolledFD*>(events[i].data.ptr);
            // Log::debug("EPoll::wait: fd=%d\n", fd->fd);
            if (fdsToDelete.find(fd)==fdsToDelete.end()) {
                fd->handleEvents(events[i].events);
            }
        }
        processing = false;
    
        while(!fdsToDelete.empty()) {
            set<PolledFD*>::iterator i = fdsToDelete.begin();
            delete *i;
            fdsToDelete.erase(i);
        }
    } else if (errno==EINTR) {
        a = 0;
    }

    return a;
}

//------------------------------------------------------------------------------
