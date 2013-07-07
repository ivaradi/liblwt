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

#ifndef LWT_DATAWRITER_H
#define LWT_DATAWRITER_H
//------------------------------------------------------------------------------

#include "BufferedWriter.h"

#include <string>

#include <cstring>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A writer that wraps a stream and provides the writing of data
 * of certain basic types. It is designed to be used with an
 * underlying stream that blocks when not all data can be written
 * immediately.
 *
 * The template parameter Stream should be a class with the
 * following function:
 * - bool write(const void* src, size_t length): write exactly the given
 *   number of bytes or fail.
 * - bool flush(): force the flushing of data written so far if some
 *   buffering is in effect.
 */
template <class Stream>
class DataWriterTemplate
{
private:
    /**
     * The stream to use to write the data.
     */
    Stream& stream;

    /**
     * Indicate if the stream is valid.
     */
    bool valid;

public:
    /**
     * Construct the data writer for the given stream.
     */
    DataWriterTemplate(Stream& stream);

    /**
     * Check if the stream is valid or not.
     */
    operator bool() const;

    /**
     * Write the given buffer.
     */
    bool write(const void* src, size_t length);

    /**
     * Flush the underlying stream.
     */
    bool flush();

    /**
     * Write a signed, 8-bit value
     */
    bool writeS8(int8_t x);

    /**
     * Write an unsigned, 8-bit value
     */
    bool writeU8(uint8_t x);

    /**
     * Write a signed, 16-bit value
     */
    bool writeS16(int16_t x);

    /**
     * Write an unsigned, 16-bit value
     */
    bool writeU16(uint16_t x);

    /**
     * Write a signed, 32-bit value
     */
    bool writeS32(int32_t x);

    /**
     * Write an unsigned, 32-bit value
     */
    bool writeU32(uint32_t x);

    /**
     * Write a signed, 64-bit value
     */
    bool writeS64(int64_t x);

    /**
     * Write an unsigned, 64-bit value
     */
    bool writeU64(uint64_t x);

    /**
     * Write a float value.
     */
    bool writeFloat(float x);

    /**
     * Write a double value.
     */
    bool writeDouble(double x);

    /**
     * Write a long double value.
     */
    bool writeLongDouble(long double x);

    /**
     * Write a length value. This a 32-bit value written as a sequence
     * of bytes of variable length. A bytes's lower 7 bits contain
     * parts of the value. The MSB indicates if another byte is left.
     */
    bool writeLength(size_t length);

    /**
     * Write a string. First the string's length is written as a
     * length (see writeLength()), then the characters of the string
     * without the trailing 0.
     */
    bool writeString(const char* s, size_t length);

    /**
     * Write a string. First the string's length is written as a
     * length (see writeLength()), then the characters of the string
     * without the trailing 0.
     */
    bool writeString(const std::string& s);

    /**
     * Write a string. First the string's length is written as a
     * length (see writeLength()), then the characters of the string
     * without the trailing 0.
     */
    bool writeString(const char* s);
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * The default data writer.
 */
class DataWriter : public DataWriterTemplate<BufferedWriter>
{
public:
    /**
     * Construct the reader.
     */
    DataWriter(BufferedWriter& stream);
};

//------------------------------------------------------------------------------
// Template definitions
//------------------------------------------------------------------------------

template <class Stream>
inline DataWriterTemplate<Stream>::DataWriterTemplate(Stream& stream) :
    stream(stream),
    valid(true)
{
}

//------------------------------------------------------------------------------

template <class Stream>
inline DataWriterTemplate<Stream>::operator bool() const
{
    return valid;
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::write(const void* src, size_t length)
{
    if (valid) {
        valid = stream.write(src, length);
    }
    return valid;
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::flush()
{
    if (valid) {
        valid = stream.flush();
    }
    return valid;
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeS8(int8_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeU8(uint8_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeS16(int16_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeU16(uint16_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeS32(int32_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeU32(uint32_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeS64(int64_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeU64(uint64_t x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeFloat(float x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeDouble(double x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeLongDouble(long double x)
{
    return write(&x, sizeof(x));
}

//------------------------------------------------------------------------------

template <class Stream>
bool DataWriterTemplate<Stream>::writeLength(size_t length)
{
    do {
        uint8_t x = length&0x7f;
        length >>= 7;
        if (length>0) x |= 0x80;
        if (!writeU8(x)) return false;
    } while(length>0);
    return true;
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeString(const char* s, size_t length)
{
    if (!writeLength(length)) return false;
    else return write(s, length);
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeString(const std::string& s)
{
    return writeString(s.c_str(), s.length());
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataWriterTemplate<Stream>::writeString(const char* s)
{
    return writeString(s, strlen(s));
}

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline DataWriter::DataWriter(BufferedWriter& stream) :
    DataWriterTemplate<BufferedWriter>(stream)
{
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_DATAWRITER_H

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
