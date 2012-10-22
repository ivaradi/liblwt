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

#ifndef LWT_BLOCKEDTHREAD_H
#define LWT_BLOCKEDTHREAD_H
//------------------------------------------------------------------------------

#include "Thread.h"

#include <cassert>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * Reference to a blocked thread. If a thread is set to it, that
 * thread is blocked and can only be unblocked via this reference. If
 * this reference is deleted, the thread is unblocked. If the thread
 * is deleted, its pointer is removed from this reference. A thread
 * can be blocked only on one such reference.
 */
class BlockedThread
{
public:
    /**
     * The for the "result" of a blocking
     */
    typedef enum {
        /// The thread was unblocked normally
        UNBLOCKED,

        /// The block was cancelled
        CANCELLED
    } result_t;

private:
    /**
     * The thread being blocked.
     */
    Thread* thread;

    /**
     * The unblock result.
     */
    result_t result;
    
public:
    /**
     * Construct the blocked thread reference.
     */
    BlockedThread();

    /**
     * Destroy the blocked thread reference.
     */
    ~BlockedThread();

    /**
     * Block the current thread via this reference.
     */
    result_t blockCurrent();

    /**
     * Check if we are blocking.
     */
    bool isBlocked() const;

    /**
     * Unblock the thread referenced by us. The reference will be
     * cleared both from here and the thread.
     *
     * @return if we could unblock a thread
     */
    bool unblock(result_t r = UNBLOCKED);

    /**
     * Cancel the current blocking
     */
    bool cancel();

private:
    /**
     * Set the given thread
     */
    void setThread(Thread* t);

    /**
     * Clear the thread
     */
    void clearThread();

    friend class Thread;
};

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

inline BlockedThread::BlockedThread() :
    thread(0)
{
}

//------------------------------------------------------------------------------

inline BlockedThread::~BlockedThread()
{
    unblock();
}

//------------------------------------------------------------------------------

inline BlockedThread::result_t BlockedThread::blockCurrent()
{
    Thread::block(this);
    return result;
}

//------------------------------------------------------------------------------

inline bool BlockedThread::isBlocked() const
{
    return thread != 0;
}

//------------------------------------------------------------------------------

inline bool BlockedThread::unblock(result_t r)
{
    result = r;

    if (!isBlocked()) return false;

    thread->unblock();

    return true;
}

//------------------------------------------------------------------------------

inline bool BlockedThread::cancel()
{
    return unblock(CANCELLED);
}

//------------------------------------------------------------------------------

inline void BlockedThread::setThread(Thread* t)
{
    assert(thread==0);
    thread = t;
}

//------------------------------------------------------------------------------

inline void BlockedThread::clearThread()
{
    thread = 0;
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_BLOCKEDTHREAD_H

// Local variables:
// mode: c++
// End:
