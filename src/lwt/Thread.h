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

#ifndef LWT_THREAD_H
#define LWT_THREAD_H
//------------------------------------------------------------------------------

#include "Context.h"

#include <string>

//------------------------------------------------------------------------------

namespace lwt { class Thread; }

//------------------------------------------------------------------------------

extern "C" void startThread(lwt::Thread*, lwt::Context&) __attribute__((used));

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

class BlockedThread;

//------------------------------------------------------------------------------

/**
 * The base class for threads.
 */
class Thread
{
private:
    /**
     * Schedule the execution of the next thread or the scheduler.
     */
    static void schedule();

    /**
     * The current thread
     */
    static Thread* current;

public:
    /**
     * Get the current thread
     */
    static Thread* getCurrent();

private:
    /**
     * Block the current thread with the given blocked thread reference.
     */
    static void block(BlockedThread* blocker);

private:
    /**
     * The next element in a list of threads
     */
    Thread* next;

    /**
     * The previous element in a list of threads
     */
    Thread* previous;

    /**
     * The top of the thread stack.
     */
    unsigned char* stackTop;

    /**
     * The context of the thread
     */
    Context context;

    /**
     * Indicate if the thread is joinable. A joinable thread should be
     * joined by another thread and then deleted. A non-joinable (detached)
     * thread is deleted when its run() function quits.
     */
    bool joinable;

    /**
     * The address of a blocked thread reference that we are blocking
     * on. 
     */
    BlockedThread* blocker;
    
    /**
     * Indicate if the thread's exectuion has finished.
     */
    bool finished;

    /**
     * The thread joining this one.
     */
    BlockedThread* joiner;

    /**
     * The thread we are joining if any.
     */
    Thread* joined;
    
    /**
     * The log context. Defaults to Thread[<address>].
     */
    std::string logContext;

protected:
    /**
     * Construct the thread.
     */
    Thread(bool joinable = false);

    /**
     * Destroy the thread. If the thread is in the ready list it will
     * be removed from it.
     *
     * For each file descriptor the thread may be associated with in
     * some way, the destructor should either delete the file
     * descriptor or call its threadDeleted() member function.
     */
    virtual ~Thread();

    /**
     * Set the log context
     */
    void setLogContext(const std::string& context);

    /**
     * The operation of the thread.
     */
    virtual void run() = 0;

public:
    /**
     * Join this thread.
     *
     * @return if the joining was successul. If it is not successful
     * that means that this thread been deleted while it was being
     * joined. 
     */
    bool join();

private:
    /**
     * Unblock the thread
     */
    void unblock();

    /**
     * Append the thread to the list of threads with the given first and
     * last elements.
     */
    void append(Thread*& first, Thread*& last);

    /**
     * Remove the thread from the list of threads with the given first
     * and last elements.
     */
    void remove(Thread*& first, Thread*& last);

    /**
     * Resume the execution of this thread
     */
    void resume();

    /**
     * Append the thread to the ready list of the scheduler.
     */
    void appendReady();

    /**
     * Finalize the thread. If the thread is joinable, its finished
     * member variable will be set and if there is joiner waiting for
     * it, it will be unblocked.
     *
     * @return if the thread is joinable
     */
    bool finalize();
    
    friend void ::startThread(Thread*, lwt::Context&);
    friend class Scheduler;
    friend class BlockedThread;
    friend class Log;
};

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline Thread* Thread::getCurrent()
{
    return current;
}

//------------------------------------------------------------------------------

inline void Thread::setLogContext(const std::string& context)
{
    logContext = context;
}

//------------------------------------------------------------------------------

inline void Thread::resume()
{
    restoreContext(context, 1);
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_THREAD_H

// Local variables:
// mode: c++
// End:
