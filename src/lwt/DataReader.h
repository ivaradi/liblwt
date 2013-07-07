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

#ifndef LWT_DATAREADER_H
#define LWT_DATAREADER_H
//------------------------------------------------------------------------------

#include "BufferedReader.h"

#include <inttypes.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A reader that wraps a stream and provides the reading of data
 * of certain basic types. It is designed to be used with an
 * underlying stream that blocks when more data is needed, but none is
 * available (yet).
 *
 * The template parameter Stream should be a class with the
 * following function:
 * - bool read(void* dest, size_t length): read exactly the given
 *   number of bytes into the buffer or fail.
 */
template <class Stream>
class DataReaderTemplate
{
private:
    /**
     * The underlying stream.
     */
    Stream& stream;

    /**
     * Indicate if the stream is valid.
     */
    bool valid;

public:
    /**
     * Construct the data reader for the given stream.
     */
    DataReaderTemplate(Stream& stream);

    /**
     * Check if the stream is valid or not.
     */
    operator bool() const;

    /**
     * Read the given number of bytes into the given buffer.
     *
     * @return if the given number of bytes could be read.
     */
    bool read(void* dest, size_t length);

    /**
     * Template for simple, type-specific reading.
     */
    template <typename T> T readT(T defaultValue = 0);

    /**
     * Read a signed, 8-bit value.
     */
    int8_t readS8();

    /**
     * Read an unsigned, 8-bit value.
     */
    uint8_t readU8();

    /**
     * Read a signed, 16-bit value.
     */
    int16_t readS16();

    /**
     * Read an unsigned, 16-bit value.
     */
    uint16_t readU16();

    /**
     * Read a signed, 32-bit value.
     */
    int32_t readS32();

    /**
     * Read an unsigned, 32-bit value.
     */
    uint32_t readU32();

    /**
     * Read a signed, 64-bit value.
     */
    int64_t readS64();

    /**
     * Read an unsigned, 64-bit value.
     */
    uint64_t readU64();

    /**
     * Read a single-precision floating point value.
     */
    float readFloat();

    /**
     * Read a double-precision floating point value.
     */
    double readDouble();

    /**
     * Read a long double-precision floating point value.
     */
    long double readLongDouble();

    /**
     * Read a length value. It is at most 32 bits, represented by a
     * variable number of bytes for space efficiency.
     */
    size_t readLength();

    /**
     * Read a string.
     */
    std::string readString();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * The default data reader.
 */
class DataReader : public DataReaderTemplate<BufferedReader>
{
public:
    /**
     * Construct the reader.
     */
    DataReader(BufferedReader& stream);
};

//------------------------------------------------------------------------------
// Template definitions
//------------------------------------------------------------------------------

template <class Stream>
DataReaderTemplate<Stream>::DataReaderTemplate(Stream& stream) :
    stream(stream),
    valid(true)
{
}

//------------------------------------------------------------------------------

template <class Stream>
inline DataReaderTemplate<Stream>::operator bool() const
{
    return valid;
}

//------------------------------------------------------------------------------

template <class Stream>
inline bool DataReaderTemplate<Stream>::read(void* dest, size_t length)
{
    if (valid) {
        valid = stream.read(dest, length);
    }
    return valid;
}

//------------------------------------------------------------------------------

template <class Stream>
template <typename T> inline T DataReaderTemplate<Stream>::readT(T defaultValue)
{
    T value = defaultValue;
    read(&value, sizeof(value));
    return value;
}

//------------------------------------------------------------------------------

template <class Stream>
inline int8_t DataReaderTemplate<Stream>::readS8()
{
    return readT<int8_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline uint8_t DataReaderTemplate<Stream>::readU8()
{
    return readT<uint8_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline int16_t DataReaderTemplate<Stream>::readS16()
{
    return readT<int16_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline uint16_t DataReaderTemplate<Stream>::readU16()
{
    return readT<uint16_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline int32_t DataReaderTemplate<Stream>::readS32()
{
    return readT<int32_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline uint32_t DataReaderTemplate<Stream>::readU32()
{
    return readT<uint32_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline int64_t DataReaderTemplate<Stream>::readS64()
{
    return readT<int64_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline uint64_t DataReaderTemplate<Stream>::readU64()
{
    return readT<uint64_t>(0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline float DataReaderTemplate<Stream>::readFloat()
{
    return readT<float>(0.0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline double DataReaderTemplate<Stream>::readDouble()
{
    return readT<double>(0.0);
}

//------------------------------------------------------------------------------

template <class Stream>
inline long double DataReaderTemplate<Stream>::readLongDouble()
{
    return readT<long double>(0.0);
}

//------------------------------------------------------------------------------

template <class Stream>
size_t DataReaderTemplate<Stream>::readLength()
{
    size_t length = 0;

    uint8_t d = 0;
    do {
        d = readU8();
        length <<= 7;
        length |= d&0x7f;
    } while( (d&0x80)==0x80 && valid );

    return length;
}

//------------------------------------------------------------------------------

template <class Stream>
std::string DataReaderTemplate<Stream>::readString()
{
    size_t length = readLength();
    if (!*this) return "";

    char* buffer = new char[length];
    if (read(buffer, length)) {
        std::string str(buffer, length);
        delete buffer;
        return str;
    } else {
        delete buffer;
        return "";
    }
}

//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline DataReader::DataReader(BufferedReader& stream) :
    DataReaderTemplate<BufferedReader>(stream)
{
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_DATAREADER_H

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
