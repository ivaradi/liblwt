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

#ifndef LWT_POLLEDFD_H
#define LWT_POLLEDFD_H
//------------------------------------------------------------------------------

#include <inttypes.h>

#include <set>

#include <cstdarg>

#include <unistd.h>
#include <sys/ioctl.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * Wrapper for a file descriptor that is being polled.
 */
class PolledFD
{
private:
    /**
     * Type for the instances
     */
    typedef std::set<PolledFD*> instances_t;

    /**
     * The set of all polled FDs
     */
    static instances_t instances;

public:
    /**
     * Update the event bits of all polled FDs
     */
    static int updateAllEvents(uint32_t& events);

protected:
    /**
     * The file descriptor 
     */
    int fd;

    /**
     * The current events.
     */
    uint32_t currentEvents;

    /**
     * Requested events
     */
    uint32_t requestedEvents;

public:
    /**
     * Construct the file descriptor and add it to the poll with the
     * given events, if events is not 0.
     */
    PolledFD(int fd = -1, uint32_t events = 0);

protected:
    /**
     * Destroy the file descriptor by removing it from the poll, and
     * closing.
     */
    virtual ~PolledFD();
    
public:
    /**
     * Set the given event bits 
     */
    void setEvents(uint32_t events);
     
    /**
     * Clear the given event bits
     */
    void clearEvents(uint32_t events);
    
    /**
     * Get the requested events of the socket.
     */
    uint32_t getRequestedEvents() const;

    /**
     * Read from the file descriptor.
     */
    ssize_t read(void* buf, size_t count);

    /**
     * Write into the file descriptor.
     */
    ssize_t write(const void* buf, size_t count);

    /**
     * Perform an IOCTL call on the file descriptor
     */
    int ioctl(int request, ...);

    /**
     * Close the file descriptor. The requested events are set to 0
     */
    int close();

    /**
     * Clear the file descriptor. The old one will be removed from epoll and
     * closed. It must be followed by a call to setFD() with a new file
     * descriptor.
     */
    void clearFD();

    /**
     * Set a new file descriptor.
     */
    void setFD(int newFD, uint32_t events);

protected:
    /**
     * Handle the event arriving for this file descriptor. 
     */
    virtual void handleEvents(uint32_t events) = 0;

    /**
     * Update the event bits.
     */
    virtual int updateEvents(uint32_t& events);

    friend class EPoll;
};

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

#include "EPoll.h"

#include <fcntl.h>
#include <unistd.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

inline PolledFD::PolledFD(int fd, uint32_t events) :
    fd(fd),
    currentEvents(0),
    requestedEvents(events)
{
    if (fd>=0) {
        fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)|O_NONBLOCK);
    } else {
        requestedEvents = 0;
    }
    instances.insert(this);
}

//------------------------------------------------------------------------------

inline PolledFD::~PolledFD()
{
    EPoll::get().remove(this);
    if (fd>=0) ::close(fd);
    instances.erase(this);
}

//------------------------------------------------------------------------------

inline void PolledFD::setEvents(uint32_t events)
{
    requestedEvents |= events;
}
     
//------------------------------------------------------------------------------

inline void PolledFD::clearEvents(uint32_t events)
{
    requestedEvents &= ~events;
}

//------------------------------------------------------------------------------

inline uint32_t PolledFD::getRequestedEvents() const
{
    return requestedEvents;
}

//------------------------------------------------------------------------------

inline ssize_t PolledFD::read(void* buf, size_t count)
{
    return ::read(fd, buf, count);
}

//------------------------------------------------------------------------------

inline ssize_t PolledFD::write(const void* buf, size_t count)
{
    return ::write(fd, buf, count);
}

//------------------------------------------------------------------------------

inline int PolledFD::ioctl(int request, ...)
{
    va_list ap;
    va_start(ap, request);
    int result = ::ioctl(fd, request, va_arg(ap, void*));
    va_end(ap);
    return result;
}

//------------------------------------------------------------------------------

inline int PolledFD::close()
{
    int a = ::close(fd);
    
    if (a==0) {
        fd = -1;
        requestedEvents = 0;
    }

    return a;
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_POLLEDFD_H

// Local variables:
// mode: c++
// End:
