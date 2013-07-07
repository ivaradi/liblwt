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

#ifndef LWT_BUFFEREDWRITER_H
#define LWT_BUFFEREDWRITER_H
//------------------------------------------------------------------------------

#include "ThreadedSocket.h"

#include <cstdlib>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A writer that maintains a buffer and writes it to the underlying
 * stream only if the buffer is full, or if flushing is requested
 * explicitly.
 *
 * The template parameter Stream should be a class with the
 * following function:
 * - ssize_t write(const void* src, size_t length): write at most the
 *   given number of bytes, but at least 1 byte.
 */
template <class Stream, size_t bufferSize = 16384>
class BufferedWriterTemplate
{
private:
    /**
     * The stream to use for actual writing.
     */
    Stream& stream;

    /**
     * The buffer to fill with data.
     */
    unsigned char buffer[bufferSize];

    /**
     * The number of bytes written so far to the buffer.
     */
    size_t size;

public:
    /**
     * Construct the writer.
     */
    BufferedWriterTemplate(Stream& stream);

    /**
     * Write the given bytes in the buffer.
     *
     * @return if all bytes could be written.
     */
    bool write(const void* src, size_t length);

    /**
     * Flush the buffer to the underlying stream.
     */
    bool flush();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * The default buffered writer.
 */
class BufferedWriter : public BufferedWriterTemplate<ThreadedSocket>
{
public:
    /**
     * Construct the writer.
     */
    BufferedWriter(ThreadedSocket& socket);
};

//------------------------------------------------------------------------------
// Template definitions
//------------------------------------------------------------------------------

template <class Stream, size_t bufferSize>
inline BufferedWriterTemplate<Stream, bufferSize>::
BufferedWriterTemplate(Stream& stream) :
    stream(stream),
    size(0)
{
}

//------------------------------------------------------------------------------

template <class Stream, size_t bufferSize>
bool BufferedWriterTemplate<Stream, bufferSize>::write(const void* src,
                                                       size_t length)
{
    const unsigned char* s = reinterpret_cast<const unsigned char*>(src);
    while(length>0) {
        if (size>=bufferSize) {
            if (!flush()) return false;
        }
        size_t toCopy = std::min(length, bufferSize - size);
        memcpy(buffer + size, s, toCopy);
        size += toCopy;
        s += toCopy;
        length -= toCopy;
    }

    return true;
}

//------------------------------------------------------------------------------

template <class Stream, size_t bufferSize>
bool BufferedWriterTemplate<Stream, bufferSize>::flush()
{
    size_t offset = 0;
    while(offset<size) {
        ssize_t s = stream.write(buffer + offset, size - offset);
        if (s<=0) return false;
        offset += s;
    }
    size = 0;
    return true;
}

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline BufferedWriter::BufferedWriter(ThreadedSocket& socket) :
    BufferedWriterTemplate<ThreadedSocket>(socket)
{
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_BUFFEREDWRITER_H

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
