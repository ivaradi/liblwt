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

#ifndef LWT_STACKMANAGER_H
#define LWT_STACKMANAGER_H
//------------------------------------------------------------------------------

#include <cstdlib>

#include <set>

#include <cassert>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * Stack manager. It is instantiated with a certain stack size (which all
 * threads must use) and produces stacks of that size. The stacks can
 * be relinquished and reused. The stacks are stored on one or more
 * mmap-ed areas with protection pages in between them.
 */
class StackManager
{
private:
#if defined(__i386__) || defined(__x86_64__)
    static const size_t PAGE_SIZE = 4096;
#else
#error "Only i386 and x86-64 are supported"
#endif

    /**
     * The only instance of the stack manager
     */
    static StackManager* instance;

public:
    /**
     * Get the only instance of the stack manager
     */
    static StackManager& get();

private:
    /**
     * The size of a stack. Should be a multiple of PAGE_SIZE.
     */
    size_t stackSize;

    /**
     * The number of stacks in a single stack pool
     */
    size_t stacksPerPool;

    /**
     * The addresses of the mmaped pools.
     */
    std::set<unsigned char*> pools;

    /**
     * The address of the first free stack. It points to the topmost pointer of
     * the stack (i.e. stack base + stackSize - sizeof(void*))
     */
    unsigned char* firstFreeStack;

public:
    /**
     * Construct the stack manager with the given sizes
     */
    StackManager(size_t stackSize = 16384, size_t stacksPerPool = 128);

    /**
     * Destroy the stack manager
     */
    ~StackManager();
    
    /**
     * Get the stack size
     */
    size_t getStackSize() const;

    /**
     * Acquire a new stack.
     *
     * @return the top of the stack
     */
    unsigned char* acquireStack();

    /**
     * Release the stack having the top at the given address.
     */
    void releaseStack(unsigned char* stackTop);

private:
    /**
     * Allocate a new pool. Its different areas will be protected as needed,
     */
    void allocatePool();
};

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline StackManager& StackManager::get()
{
    assert(instance!=0);
    return *instance;
}

//------------------------------------------------------------------------------

inline StackManager::StackManager(size_t stackSize, size_t stacksPerPool) :
    stackSize((stackSize+PAGE_SIZE-1)&(~(PAGE_SIZE-1))),
    stacksPerPool(stacksPerPool),
    firstFreeStack(0)
{
    assert(instance==0);
    instance = this;
}

//------------------------------------------------------------------------------

inline size_t StackManager::getStackSize() const
{
    return stackSize;
}

//------------------------------------------------------------------------------

inline unsigned char* StackManager::acquireStack()
{
    if (firstFreeStack==0) allocatePool();
    
    unsigned char* stack = firstFreeStack + sizeof(void*);
    firstFreeStack = *reinterpret_cast<unsigned char**>(firstFreeStack);
    return stack;
}

//------------------------------------------------------------------------------

inline void StackManager::releaseStack(unsigned char* stackTop)
{
    stackTop -= sizeof(void*);
    *reinterpret_cast<unsigned char**>(stackTop) = firstFreeStack;
    firstFreeStack = stackTop;
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_STACKMANAGER_H

// Local variables:
// mode: c++
// End:
