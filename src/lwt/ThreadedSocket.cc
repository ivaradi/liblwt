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

//------------------------------------------------------------------------------

#include "ThreadedSocket.h"

//------------------------------------------------------------------------------

using lwt::ThreadedSocket;

//------------------------------------------------------------------------------

int ThreadedSocket::accept(struct sockaddr* addr, socklen_t* addrlen)
{
    while(true) {
        int s = Socket::accept(addr, addrlen);
        if (s<0 && (errno==EAGAIN || errno==EWOULDBLOCK)) {
            if (!waitRead()) return -1;
        } else {
            return s;
        }
    }
}

//------------------------------------------------------------------------------

int ThreadedSocket::connect(const struct sockaddr* addr, socklen_t addrlen)
{
    int result = Socket::connect(addr, addrlen);
    if (result==0 || errno!=EINPROGRESS) return result;

    if (!waitWrite()) return -1;

    errno = getError();
    return (errno==0) ? 0 : -1;
}

//------------------------------------------------------------------------------

ssize_t ThreadedSocket::recv(void* buf, size_t len, int flags)
{
    while(true) {
        ssize_t result = Socket::recv(buf, len, flags);
        if (result<0 && (errno==EAGAIN || errno==EWOULDBLOCK)) {
            if (!waitRead()) return 0;
        } else {
            return result;
        }
    }
}

//------------------------------------------------------------------------------

ssize_t ThreadedSocket::send(const void* buf, size_t len, int flags)
{
    while(true) {
        ssize_t result = Socket::send(buf, len, flags);
        if (result<0 && (errno==EAGAIN || errno==EWOULDBLOCK)) {
            if (!waitWrite()) return 0;
        } else {
            return result;
        }
    }
}

//------------------------------------------------------------------------------

bool ThreadedSocket::sendAll(const void* buf, size_t len, int flags)
{
    const char* data = reinterpret_cast<const char*>(buf);
    while(len>0) {
        ssize_t result = send(data, len, flags);
        if (result<=0) return false;
        len -= result;
        data += result;
    }
    return true;
}

//------------------------------------------------------------------------------

int ThreadedSocket::shutdown(int how)
{
    int a = Socket::shutdown(how);
    if (a==0) {
        if (how==SHUT_RD || how==SHUT_RDWR) readWaiter.unblock();
        if (how==SHUT_WR || how==SHUT_RDWR) writeWaiter.unblock();
    }
    return a;
}

//------------------------------------------------------------------------------
