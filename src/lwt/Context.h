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

#ifndef LWT_CONTEXT_H
#define LWT_CONTEXT_H
//------------------------------------------------------------------------------

#include <inttypes.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

// Register context

//------------------------------------------------------------------------------

#if defined(__i386__)

//------------------------------------------------------------------------------

struct Context
{
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t eip;
};

//------------------------------------------------------------------------------

#elif defined(__x86_64__)

//------------------------------------------------------------------------------

struct Context
{
    uint64_t rbx;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t rip;
};

//------------------------------------------------------------------------------

#else

#error "Implemented for i386 and x86-64 only"

#endif

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------

/**
 * Save the current context and return 0. If restoreContext() is
 * called later with the same context, return its retval argument
 * (which sould be nonzero!)
 */
extern "C" int saveContext(lwt::Context& context) __attribute__((returns_twice));

/**
 * Restore the given context with the given return value which should
 * be nonzero.
 */
extern "C" void restoreContext(lwt::Context& context, int retval) __attribute__((noreturn));

//------------------------------------------------------------------------------
#endif // LWT_CONTEXT_H

// Local variables:
// mode: c++
// End:
