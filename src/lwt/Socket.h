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

#ifndef LWT_SOCKET_H
#define LWT_SOCKET_H
//------------------------------------------------------------------------------

#include "PolledFD.h"

#include <sys/socket.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A socket
 */
class Socket : public PolledFD
{
public:
    /**
     * Construct the socket for the given file descriptor
     */
    Socket(int fd);

    /**
     * Construct the socket 
     */
    Socket(int domain, int type, int protocol = 0);

protected:
    /**
     * Destructor is protected to avoid inadvertent deletion
     */
    virtual ~Socket();

public:
    /**
     * Get the given socket option
     */
    int getsockopt(int level, int optname, void* optval, socklen_t* optlen);

    /**
     * Set the given socket option
     */
    int setsockopt(int level, int optname, const void* optval, socklen_t optlen);

    /**
     * Get the error code using getsockopt
     */
    int getError();

    /**
     * Bind the socket to the given address
     */
    int bind(const struct sockaddr* addr, socklen_t addrlen);

    /**
     * Start listening on the socket
     */
    int listen(int backlog = 5);

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
     * Shut down the socket
     */
    int shutdown(int how);
};

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline Socket::Socket(int fd) :
    PolledFD(fd)
{
}

//------------------------------------------------------------------------------

inline Socket::Socket(int domain, int type, int protocol) :
    PolledFD(::socket(domain, type, protocol))
{
}

//------------------------------------------------------------------------------

inline Socket::~Socket()
{
}

//------------------------------------------------------------------------------

inline int Socket::getsockopt(int level, int optname, 
                              void* optval, socklen_t* optlen)
{
    return ::getsockopt(fd, level, optname, optval, optlen);
}

//------------------------------------------------------------------------------

inline int Socket::setsockopt(int level, int optname, 
                              const void* optval, socklen_t optlen)
{
    return ::setsockopt(fd, level, optname, optval, optlen);
}

//------------------------------------------------------------------------------

inline int Socket::bind(const struct sockaddr* addr, socklen_t addrlen)
{
    return ::bind(fd, addr, addrlen);
}

//------------------------------------------------------------------------------

inline int Socket::listen(int backlog)
{
    return ::listen(fd, backlog);
}

//------------------------------------------------------------------------------

inline int Socket::accept(struct sockaddr* addr, socklen_t* addrlen)
{
    return ::accept(fd, addr, addrlen);
}

//------------------------------------------------------------------------------

inline int Socket::connect(const struct sockaddr* addr, socklen_t addrlen)
{
    return ::connect(fd, addr, addrlen);
}

//------------------------------------------------------------------------------

inline ssize_t Socket::recv(void* buf, size_t len, int flags)
{
    return ::recv(fd, buf, len, flags);
}

//------------------------------------------------------------------------------

inline ssize_t Socket::send(const void* buf, size_t len, int flags)
{
    return ::send(fd, buf, len, flags);
}

//------------------------------------------------------------------------------

inline int Socket::shutdown(int how)
{
    return ::shutdown(fd, how);
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_SOCKET_H

// Local variables:
// mode: c++
// End:
