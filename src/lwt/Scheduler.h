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

#ifndef LWT_SCHEDULER_H
#define LWT_SCHEDULER_H
//------------------------------------------------------------------------------

#include "Context.h"
#include "Thread.h"
#include "StackManager.h"
#include "EPoll.h"

#include <memory>

#include <cassert>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * The scheduler for the threads
 */
class Scheduler
{
private:
    /**
     * The only instance of the scheduler
     */
    static Scheduler* instance;
    
public:
    /**
     * Get the only instance of the scheduler
     */
    static Scheduler& get();

private:
    /**
     * Our stack manager
     */
    StackManager stackManager;
    
    /**
     * Our epoll wrapper
     */
    std::unique_ptr<EPoll> epoll;

    /**
     * The first ready thread
     */
    Thread* readyFirst;

    /**
     * The last ready thread
     */
    Thread* readyLast;

    /**
     * The context of the scheduler.
     */
    Context context;

public:
    /**
     * Construct the scheduler.
     */
    Scheduler(size_t stackSize = 16384, size_t stacksPerPool = 128,
              std::unique_ptr<EPoll> epoll = nullptr);

    /**
     * Destroy the scheduler.
     */
    ~Scheduler();

    /**
     * Run the scheduler.
     */
    void run();

private:
    /**
     * Append the given thread to the ready list
     */
    void appendReady(Thread* thread);

    /**
     * Remove the given thread from the ready list if that thread is
     * in the ready list
     */
    void removeReady(Thread* thread);

    /**
     * Process the ready list.
     */
    void processReady();

    /**
     * Resume the scheduler context
     */
    void resume();

    /**
     * Schedule. If there is a thread that is ready for execution,
     * that thread will be resumed. Otherwise the scheduler context is
     * resumed.
     */
    void schedule();

    friend class Thread;
};

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline Scheduler& Scheduler::get()
{
    assert(instance!=0);
    return *instance;
}

//------------------------------------------------------------------------------

inline Scheduler::Scheduler(size_t stackSize, size_t stacksPerPool,
                            std::unique_ptr<EPoll> epoll) :
    stackManager(stackSize, stacksPerPool),
    epoll(epoll ? std::move(epoll) : std::make_unique<EPoll>()),
    readyFirst(0),
    readyLast(0)
{
    assert(instance==0);
    instance = this;
}

//------------------------------------------------------------------------------

inline Scheduler::~Scheduler()
{
    assert(readyFirst==0);
    assert(readyLast==0);
    assert(instance==this);
    instance = 0;
}

//------------------------------------------------------------------------------

inline void Scheduler::appendReady(Thread* thread)
{
    thread->append(readyFirst, readyLast);
}

//------------------------------------------------------------------------------

inline void Scheduler::removeReady(Thread* thread)
{
    if (thread->next!=0) {
        thread->remove(readyFirst, readyLast);
    }
}

//------------------------------------------------------------------------------

inline void Scheduler::resume()
{
    restoreContext(context, 1);
}

//------------------------------------------------------------------------------

inline void Scheduler::schedule()
{
    processReady();
    resume();
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_SCHEDULER_H

// Local variables:
// mode: c++
// End:
