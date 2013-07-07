// Copyright (c) 2013 by István Váradi

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

#ifndef LWT_BUFFEREDREADER_H
#define LWT_BUFFEREDREADER_H
//------------------------------------------------------------------------------

#include "ThreadedSocket.h"

#include <cstring>
#include <cstdlib>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A reader that manages a buffer and servers read requests from
 * that.
 *
 * The template parameter Stream should be a class with the
 * following function:
 * - ssize_t read(void* dest, size_t length): read at most the given
 *   number of bytes into the buffer, but try to read at least one
 *   byte or fail. It should block, if no data is available yet.
 */
template <class Stream, size_t bufferSize = 16384>
class BufferedReaderTemplate
{
private:
    /**
     * The stream to read actual data from.
     */
    Stream& stream;

    /**
     * The buffer.
     */
    unsigned char buffer[bufferSize];

    /**
     * The number of bytes in the buffer.
     */
    size_t size;

    /**
     * The reading offset within the buffer.
     */
    size_t offset;

public:
    /**
     * Construct the reader for the given stream.
     */
    BufferedReaderTemplate(Stream& stream);

    /**
     * Read the exactly the given number of bytes into the given
     * buffer.
     *
     * If the underlying stream fails to provide as many bytes, it fails.
     */
    bool read(void* dest, size_t length);
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * The default buffered reader.
 */
class BufferedReader : public BufferedReaderTemplate<ThreadedSocket>
{
public:
    /**
     * Construct the reader.
     */
    BufferedReader(ThreadedSocket& socket);
};

//------------------------------------------------------------------------------
// Template definitions
//------------------------------------------------------------------------------

template <class Stream, size_t bufferSize>
inline BufferedReaderTemplate<Stream, bufferSize>::
BufferedReaderTemplate(Stream& stream) :
    stream(stream),
    size(0),
    offset(0)
{
}

//------------------------------------------------------------------------------

template <class Stream, size_t bufferSize>
bool BufferedReaderTemplate<Stream, bufferSize>::read(void* dest, size_t length)
{
    unsigned char* d = reinterpret_cast<unsigned char*>(dest);
    while(length>0) {
        if (offset>=size) {
            ssize_t s = stream.read(buffer, bufferSize);
            if (s<=0) return false;
            size = s;
            offset = 0;
        }
        size_t toCopy = std::min(length, size - offset);
        memcpy(d, buffer + offset, toCopy);
        d += toCopy;
        length -= toCopy;
        offset += toCopy;
    }
    return true;
}

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline BufferedReader::BufferedReader(ThreadedSocket& socket) :
    BufferedReaderTemplate<ThreadedSocket>(socket)
{
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_BUFFEREDREADER_H

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
