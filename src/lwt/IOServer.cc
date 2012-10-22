// Copyright (c) 2012 by István Váradi

// This file is part of liblwt, a lightweigtht cooperative threading library

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

#include "IOServer.h"

#include "ThreadedFD.h"
#include "EPoll.h"

#include <pthread.h>

//------------------------------------------------------------------------------

using lwt::IOServer;

//------------------------------------------------------------------------------

class IOServer::Worker
{
private:
    /**
     * Create a pipe. 
     *
     * @param returnWriting if true, the writing end will be returned,
     * otherwise the reading end
     * @param otherEnd will contain the other end of the pipe, than
     * what is returned
     */
    static int createPipe(bool returnWriting, int& otherEnd);

    /**
     * The start routine for the thread.
     */
    static void* startThread(void* arg);

    /**
     * The POSIX thread running this worker.
     */
    pthread_t thread;
    
    /**
     * The writing end of the pipe through which operations are sent to
     * this worker.
     */
    ThreadedFD* toWorker;

    /**
     * The reading end of the pipe via which results are received from
     * this worker.
     */
    ThreadedFD* fromWorker;

    /**
     * The reading end of the pipe through which operations are
     * received from the clients.
     */
    int fromClient;
    
    /**
     * The writing end of the pipe through which results are
     * sent to the clients.
     */
    int toClient;

public:
    /**
     * Construct the worker. It starts the thread.
     */
    Worker();

    /**
     * Destroy the worker. It joins the thread.
     */
    ~Worker();

    /**
     * Start the given operation on the worker. This one writes
     * pointer to the thread.
     *
     * @return if the pointer could be written
     */
    bool start(Operation* operation);
    
    /**
     * Wait the result of the last operation sent.
     *
     * @return the operation pointer read, or 0 on error.
     */
    Operation* waitResult();

    /**
     * Execute the given operation. This is a combination of start and
     * waitResult. 
     *
     * @return if the operation could be written and executed.
     */
    bool execute(Operation* operation);
    
private:
    /**
     * Perform the actual operation of the thread.
     */
    void run();
};

//------------------------------------------------------------------------------

int IOServer::Worker::createPipe(bool returnWriting, int& otherEnd)
{
    int fds[2];
    if (pipe(fds)<0) {
        otherEnd = -1;
        return -1;
    } else {
        otherEnd = fds[returnWriting ? 0 : 1];
        return fds[returnWriting ? 1 : 0];
    }
}

//------------------------------------------------------------------------------

void* IOServer::Worker::startThread(void* arg)
{
    Worker* worker = reinterpret_cast<Worker*>(arg);
    worker->run();
    return 0;
}

//------------------------------------------------------------------------------

IOServer::Worker::Worker() :
    toWorker(new ThreadedFD(createPipe(true, fromClient))),
    fromWorker(new ThreadedFD(createPipe(false, toClient)))
{
    pthread_create(&thread, 0, &startThread, this);
}

//------------------------------------------------------------------------------

IOServer::Worker::~Worker()
{
    EPoll::get().destroy(toWorker);
    EPoll::get().destroy(fromWorker);
    pthread_join(thread, 0);
}

//------------------------------------------------------------------------------

inline bool IOServer::Worker::start(Operation* operation)
{
    return toWorker->writeAll(&operation, sizeof(operation));
}

//------------------------------------------------------------------------------

inline IOServer::Operation* IOServer::Worker::waitResult()
{
    Operation* operation = 0;
    if (fromWorker->read(&operation, sizeof(operation))!=sizeof(operation)) {
        operation = 0;
    }
    return operation;
}

//------------------------------------------------------------------------------

bool IOServer::Worker::execute(Operation* operation)
{
    if (!start(operation)) return false;

    Operation* result = waitResult();
    assert(result==operation);
    return operation==result;
}

//------------------------------------------------------------------------------

void IOServer::Worker::run()
{
    Operation* operation = 0;
    while(read(fromClient, &operation, sizeof(operation))==sizeof(operation)) {
        operation->perform();
        if (write(toClient, &operation, sizeof(operation))<0) {
            break;
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void IOServer::ErrnoOperation::perform()
{
    performErrno();
    errorNumber = errno;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

IOServer* IOServer::instance = 0;

//------------------------------------------------------------------------------

IOServer::IOServer(size_t numWorkers) :
    workers(new Worker[numWorkers])
{
    availableWorkers.reserve(numWorkers);
    for(size_t i = 0; i<numWorkers; ++i) {
        availableWorkers.push_back(workers+i);
    }
    instance = this;
}

//------------------------------------------------------------------------------

IOServer::~IOServer()
{
    instance = 0;
    delete [] workers;
}

//------------------------------------------------------------------------------

bool IOServer::execute(Operation* operation, bool canBlock)
{
    bool result = false;

    Worker* worker = getWorker(canBlock);
    if (worker!=0) {
        result = worker->execute(operation);
        putWorker(worker);
    }

    return result;
}

//------------------------------------------------------------------------------

IOServer::Worker* IOServer::getWorker(bool canBlock)
{
    while(availableWorkers.empty()) {
        if (!canBlock) return 0;
        BlockedThread* blockedThread = new BlockedThread;
        waiters.push_back(blockedThread);
        blockedThread->blockCurrent();
    }

    Worker* worker = availableWorkers.back();
    availableWorkers.pop_back();
    return worker;
}

//------------------------------------------------------------------------------

void IOServer::putWorker(Worker* worker)
{
    availableWorkers.push_back(worker);
    if (!waiters.empty()) {
        BlockedThread* blockedThread = waiters.front();
        waiters.pop_front();
        delete blockedThread;
    }
}

//------------------------------------------------------------------------------

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:

