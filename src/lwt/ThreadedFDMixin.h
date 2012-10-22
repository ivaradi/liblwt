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

#ifndef LWT_THREADEDFDMIXIN_H
#define LWT_THREADEDFDMIXIN_H
//------------------------------------------------------------------------------

#include "BlockedThread.h"

#include <cerrno>

#include <sys/epoll.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A mixin for the polled file descriptor that can be used to wait for
 * the file descriptor becoming readable or writable.
 */
template <class Super>
class ThreadedFDMixin : public Super
{
protected:
    /**
     * The thread waiting for an EPOLLIN
     */
    BlockedThread readWaiter;
    
    /**
     * The thread waiting for an EPOLLOUT
     */
    BlockedThread writeWaiter;

protected:
    /**
     * Construct a threaded file descriptor for the given file
     * descriptor
     */
    ThreadedFDMixin(int fd);

public:
    /**
     * Cancel a reading, if one is in progress.
     *
     * @return if we could cancel the reading
     */
    bool cancelRead();
    
    /**
     * Cancel a writing, if one is in progress.
     *
     * @return if we could cancel the writing
     */
    bool cancelWrite();

    /**
     * Read from the file descriptor. It blocks until data is
     * available (or an error occurs).
     */
    ssize_t read(void* buf, size_t count);

    /**
     * Write into the file descriptor. It blocks until at least some
     * of the data can be written (or an error occurs).
     */
    ssize_t write(const void* buf, size_t count);

    /**
     * Write all the given data to the file descriptor. It blocks
     * until all data can be written (or an error occurs).
     *
     * @return an indication of whether all data could be written
     */
    bool writeAll(const void* buf, size_t count);
    
    /**
     * Close the file descriptor. It unblocks any waiters.
     */
    int close();

protected:    
    /**
     * It unblocks any waiting thread.
     *
     * @see PolledFD::handleEvents
     */
    virtual void handleEvents(uint32_t events);

    /**
     * The requestedEvents will be set based on what we are blocking
     * on, and then the superclass' updateEvents will b called.
     */
    virtual int updateEvents(uint32_t& events);

    /**
     * Wait for the file descriptor becoming readable
     *
     * @return if we were unblocked normally
     */
    bool waitRead();
    
    /**
     * Wait for the file descriptor becoming writable
     *
     * @return if we were unblocked normally
     */
    bool waitWrite();
};

//------------------------------------------------------------------------------
// Template definitions
//------------------------------------------------------------------------------

template <class Super> inline ThreadedFDMixin<Super>::ThreadedFDMixin(int fd) :
    Super(fd)
{
}

//------------------------------------------------------------------------------

template <class Super> inline bool ThreadedFDMixin<Super>::cancelRead()
{
    return readWaiter.cancel();
}

//------------------------------------------------------------------------------

template <class Super> inline bool ThreadedFDMixin<Super>::cancelWrite()
{
    return writeWaiter.cancel();
}

//------------------------------------------------------------------------------

template <class Super> ssize_t 
ThreadedFDMixin<Super>::read(void* buf, size_t count)
{
    while(true) {
        ssize_t result = Super::read(buf, count);
        if (result<0 && (errno==EAGAIN || errno==EWOULDBLOCK)) {
            if (!waitRead()) return -1;
        } else {
            return result;
        }
    }
}

//------------------------------------------------------------------------------

template <class Super> ssize_t 
ThreadedFDMixin<Super>::write(const void* buf, size_t count)
{
    while(true) {
        ssize_t result = Super::write(buf, count);
        if (result<0 && (errno==EAGAIN || errno==EWOULDBLOCK)) {
            if (!waitWrite()) return -1;
        } else {
            return result;
        }
    }
}

//------------------------------------------------------------------------------

template <class Super> bool
ThreadedFDMixin<Super>::writeAll(const void* buf, size_t count)
{
    const char* data = reinterpret_cast<const char*>(buf);
    while(count>0) {
        ssize_t result = write(data, count);
        if (result<=0) return false;
        count -= result;
        data += result;
    }
    return true;
}

//------------------------------------------------------------------------------

template <class Super> inline int ThreadedFDMixin<Super>::close()
{
    int a = Super::close();

    if (a==0) {
        readWaiter.unblock();
        writeWaiter.unblock();
    }

    return a;
}

//------------------------------------------------------------------------------

template <class Super>
void ThreadedFDMixin<Super>::handleEvents(uint32_t events)
{
    if (readWaiter.isBlocked() && (events&EPOLLIN)!=0) {
        readWaiter.unblock();
    }
    if (writeWaiter.isBlocked() && (events&EPOLLOUT)!=0) {
        writeWaiter.unblock();
    }
}

//------------------------------------------------------------------------------

template <class Super>
int ThreadedFDMixin<Super>::updateEvents(uint32_t& events)
{
    Super::requestedEvents = 0;
    if (readWaiter.isBlocked()) Super::requestedEvents |= EPOLLIN;
    if (writeWaiter.isBlocked()) Super::requestedEvents |= EPOLLOUT;
    
    return Super::updateEvents(events);
}

//------------------------------------------------------------------------------

template <class Super> inline bool ThreadedFDMixin<Super>::waitRead()
{
    return readWaiter.blockCurrent()==BlockedThread::UNBLOCKED;
}

//------------------------------------------------------------------------------

template <class Super> inline bool ThreadedFDMixin<Super>::waitWrite()
{
    return writeWaiter.blockCurrent()==BlockedThread::UNBLOCKED;
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_THREADEDFDMIXIN_H

// Local variables:
// mode: c++
// End:
