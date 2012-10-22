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

#ifndef LWT_EPOLL_H
#define LWT_EPOLL_H
//------------------------------------------------------------------------------

#include <set>

#include <inttypes.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

class PolledFD;

//------------------------------------------------------------------------------

/**
 * Wrapper for the epoll functions
 */
class EPoll
{
private:
    /**
     * The only instance of the epoll helper.
     */
    static EPoll* instance;

public:
    /**
     * Get the instance of the epoll helper
     */
    static EPoll& get();

private:
    /**
     * The poll file descriptor
     */
    int epfd;

    /**
     * Indicate if we are processing events
     */
    bool processing;

    /**
     * The set of polled FDs to delete
     */
    std::set<PolledFD*> fdsToDelete;

public:
    /**
     * Construct the wrapper
     */
    EPoll();

    /**
     * Destroy the wrapper
     */
    ~EPoll();

    /**
     * Add the given file descriptor with the given event flags.
     */
    int add(PolledFD* polledFD, uint32_t events);

    /**
     * Modify the events associated with the given file descriptor
     */
    int modify(PolledFD* polledFD, uint32_t events);

    /**
     * Remove the given file descriptor from the poll
     */
    int remove(PolledFD* polledFD);

    /**
     * Destroy the given polled FD. If we are processing
     * events, it will only be put into the set containing the FDs to
     * delete. Otherwise it is deleted
     */
    void destroy(PolledFD* polledFD);

    /**
     * Wait for events with the given timeout. Any events received
     * will be processed, i.e. the corresponding file descriptors will
     * be called.
     */
    int wait(bool& hadEvents, int timeout = -1);
};

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

#include "PolledFD.h"

#include <sys/epoll.h>
#include <unistd.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

inline EPoll& EPoll::get()
{
    return *instance;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline EPoll::EPoll() :
    epfd(epoll_create(1)),
    processing(false)
{
    instance = this;    
}

//------------------------------------------------------------------------------

inline EPoll::~EPoll()
{
    close(epfd);
    instance = 0;
}

//------------------------------------------------------------------------------

inline int EPoll::add(PolledFD* polledFD, uint32_t events)
{
    struct epoll_event event;
    event.events = events;
    event.data.ptr = polledFD;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, polledFD->fd, &event);
}

//------------------------------------------------------------------------------

inline int EPoll::modify(PolledFD* polledFD, uint32_t events)
{
    struct epoll_event event;
    event.events = events;
    event.data.ptr = polledFD;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, polledFD->fd, &event);
}

//------------------------------------------------------------------------------

inline int EPoll::remove(PolledFD* polledFD)
{
    return epoll_ctl(epfd, EPOLL_CTL_DEL, polledFD->fd, 0);
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_EPOLL_H

// Local variables:
// mode: c++
// End:
