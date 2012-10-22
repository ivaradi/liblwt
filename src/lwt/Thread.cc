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

#include "BlockedThread.h"
#include "Thread.h"

#include "Scheduler.h"
#include "StackManager.h"

#include <cstdio>

//------------------------------------------------------------------------------

using lwt::Thread;
using lwt::Scheduler;
using lwt::Context;

//------------------------------------------------------------------------------

Thread* Thread::current = 0;

//------------------------------------------------------------------------------

inline void Thread::appendReady()
{
    Scheduler::get().appendReady(this);
}

//------------------------------------------------------------------------------

inline void Thread::schedule()
{
    Scheduler::get().schedule();
}

//------------------------------------------------------------------------------

void Thread::block(BlockedThread* blocker)
{
    current->blocker = blocker;
    blocker->setThread(current);
    if (saveContext(current->context)==0) {
        schedule();
    }
}

//------------------------------------------------------------------------------

extern "C" void startThread(Thread* thread, Context& callerContext)
{
    if (saveContext(thread->context)==0) {
        thread->appendReady();
        restoreContext(callerContext, 1);
    } else {
        thread->run();
        if (thread->joinable) {
            if (thread->joiner->isBlocked()) {
                thread->finished = true;
                thread->joiner->unblock();
            }
        } else {
            delete thread;
        }
        Thread::schedule();
    }
}

//------------------------------------------------------------------------------

Thread::Thread(bool joinable) :
    next(0),
    previous(0),
    stackTop(StackManager::get().acquireStack()),
    joinable(joinable),
    blocker(0),
    finished(false),
    joiner(joinable ? new BlockedThread() : 0),
    joined(0)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "Thread[%p]", this);
    logContext = buf;

    Context callerContext;
    if (saveContext(callerContext)==0) {
        uintptr_t stackTopAddress = reinterpret_cast<uintptr_t>(stackTop);
#if defined(__i386__)
        asm volatile("mov %0,%%esp" : : "g" (stackTopAddress));
        asm volatile("push %0" : : "g" (&callerContext));
        asm volatile("push %0" : : "g" (this));
        asm volatile("call startThread");
#elif defined(__x86_64__)
        asm volatile("mov %0,%%rdi" : : "g" (this));
        asm volatile("mov %0,%%rsi" : : "g" (&callerContext));
        asm volatile("mov %0,%%rsp" : : "g" (stackTopAddress));
        asm volatile("call startThread");
#else
#error "Implemented for i386 and x86_64 only"
#endif
    }
}

//------------------------------------------------------------------------------

Thread::~Thread()
{    
    assert(joiner==0 || !joiner->isBlocked());

    delete joiner;

    Scheduler::get().removeReady(this);
    
    if (blocker!=0) {
        blocker->clearThread();
        blocker = 0;
    }

    StackManager::get().releaseStack(stackTop);

    if (current==this) current = 0;
}

//------------------------------------------------------------------------------

bool Thread::join()
{
    if (!joinable) return false;

    if (finished) return true;

    current->joined = this;        
    joiner->blockCurrent();
    current->joined = 0;
    
    return true;
}

//------------------------------------------------------------------------------

void Thread::unblock()
{    
    assert(blocker!=0);
    blocker->clearThread();
    blocker = 0;
    appendReady();
}

//------------------------------------------------------------------------------

void Thread::append(Thread*& first, Thread*& last)
{
    assert(next==0 && previous==0);
    if (last==0) {
        next = previous = this;
        first = last = this;
    } else {
        previous = last;
        previous->next = this;
        next = first;
        next->previous = this;
        last = this;
    }
}

//------------------------------------------------------------------------------

void Thread::remove(Thread*& first, Thread*& last)
{
    assert(next!=0 && previous!=0);
    if (first==this && last==this) {
        first = last = 0;
    } else if (first==this) {
        last->next = first->next;
        last->next->previous = last;
        first = first->next;
    } else if (last==this) {
        first->previous = last->previous;
        first->previous->next = first;
        last = last->previous;
    } else {
        previous->next = next;
        next->previous = previous;
    }
    next = previous = 0;
}

//------------------------------------------------------------------------------
