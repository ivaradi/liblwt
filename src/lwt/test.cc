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

#include "Thread.h"
#include "Timer.h"
#include "Scheduler.h"

#include <cstdio>

//------------------------------------------------------------------------------

using lwt::StackManager;
using lwt::Scheduler;
using lwt::Timer;

//------------------------------------------------------------------------------

class TestThread : public lwt::Thread
{
public:
    TestThread();

    ~TestThread();

protected:
    virtual void run();
};

//------------------------------------------------------------------------------

TestThread::TestThread()
{
    printf("TestThread: @%p\n", this);
}

//------------------------------------------------------------------------------

TestThread::~TestThread()
{
    printf("~TestThread: @%p\n", this);
}

//------------------------------------------------------------------------------

void TestThread::run()
{
    new TestThread();
    // while(true) {
    //     printf("Hello, world!\n");
    //     Timer::sleep(1000);
    // }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int main()
{
    Scheduler scheduler;
    
    new TestThread();

    scheduler.run();
    
    return 0;
}

//------------------------------------------------------------------------------
