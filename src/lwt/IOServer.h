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

#ifndef LWT_IOSERVER_H
#define LWT_IOSERVER_H
//------------------------------------------------------------------------------

#include <cstdlib>

#include <vector>
#include <deque>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

class BlockedThread;

//------------------------------------------------------------------------------

/**
 * An I/O server that can be used to perform blocking operations.
 */
class IOServer
{
public:
    /**
     * Base class for the operations to perform.
     */
    class Operation
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~Operation();

    protected:
        /**
         * Perform the operation in the worker thread.
         */
        virtual void perform() = 0;

        friend class IOServer;
    };

    /**
     * Base class for an operation that stores the error number.
     */
    class ErrnoOperation : public Operation
    {
    private:
        /**
         * The error number.
         */
        int errorNumber;

    public:
        /**
         * Construct the operation.
         */
        ErrnoOperation();

        /**
         * Get the error number.
         */
        int getErrorNumber() const;

    protected:
        /**
         * Perform the operation by calling performErrno(), and then
         * store the returned error number.
         */
        virtual void perform();

        /**
         * Perform the operation really.
         */
        virtual void performErrno() = 0;
    };

private:
    /**
     * A worker thread.
     */
    class Worker;

    /**
     * The only instance of this server.
     */
    static IOServer* instance;

public:
    /**
     * Get the only instance of the I/O server.
     */
    static IOServer& get();

private:
    /**
     * All the workers.
     */
    Worker* workers;

    /**
     * Available workers.
     */
    std::vector<Worker*> availableWorkers;

    /**
     * The threads waiting for workers.
     */
    std::deque<BlockedThread*> waiters;

public:
    /**
     * Construct the I/O server with the given number of worker threads.
     */
    IOServer(size_t numWorkers);

    /**
     * Destroy the I/O server.
     */
    ~IOServer();

    /**
     * Execute the given operation.
     */
    bool execute(Operation* operation, bool canBlock = true);

    /**
     * Execute the given operation in non-blocking mode, i.e. if there
     * is no worker available immediately, return at once.
     */
    bool executeNonBlocking(Operation* operation);

private:
    /**
     * Get an available worker.
     */
    Worker* getWorker(bool canBlock);

    /**
     * Release a worker being used. If there are any waiters, the
     * first one is woken up.
     */
    void putWorker(Worker* worker);
};

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline IOServer::Operation::~Operation()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline IOServer::ErrnoOperation::ErrnoOperation() :
    errorNumber(0)
{
}

//------------------------------------------------------------------------------

inline int IOServer::ErrnoOperation::getErrorNumber() const
{
    return errorNumber;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline IOServer& IOServer::get()
{
    return *instance;
}

//------------------------------------------------------------------------------

inline bool IOServer::executeNonBlocking(Operation* operation)
{
    return execute(operation, false);
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_IOSERVER_H

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:

