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

#ifndef LWT_THREADEDSOCKET_H
#define LWT_THREADEDSOCKET_H
//------------------------------------------------------------------------------

#include "ThreadedFDMixin.h"
#include "Socket.h"

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A socket that can be used in threads, because it blocks execution
 */
class ThreadedSocket : public ThreadedFDMixin<Socket>
{
public:
    /**
     * Construct the socket for the given file descriptor
     */
    ThreadedSocket(int fd);

    /**
     * Construct the socket 
     */
    ThreadedSocket(int domain, int type, int protocol = 0);

protected:
    /**
     * Destructor is protected to avoid inadvertent deletion
     */
    virtual ~ThreadedSocket();

public:    
    /**
     * Accept a connection 
     */
    int accept(struct sockaddr* addr = 0, socklen_t* addrlen = 0);

    /**
     * Connect to a socket
     */
    int connect(const struct sockaddr* addr, socklen_t addrlen);

    /**
     * Receive some data into the given buffer
     */
    ssize_t recv(void* buf, size_t len, int flags = 0);

    /**
     * Send some data on the socket
     */
    ssize_t send(const void* buf, size_t len, int flags = 0);

    /**
     * Send all data, if possible
     */
    bool sendAll(const void* buf, size_t len, int flags = 0);

    /**
     * Shut down the socket
     */
    int shutdown(int how);
};

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline ThreadedSocket::ThreadedSocket(int fd) :
    ThreadedFDMixin<Socket>(fd)
{
}

//------------------------------------------------------------------------------

inline ThreadedSocket::ThreadedSocket(int domain, int type, int protocol) :
    ThreadedFDMixin<Socket>(::socket(domain, type, protocol))
{
}

//------------------------------------------------------------------------------

inline ThreadedSocket::~ThreadedSocket()
{
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_THREADEDSOCKET_H

// Local variables:
// mode: c++
// End:
