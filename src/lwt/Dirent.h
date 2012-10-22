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

#ifndef LWT_DIRENT_H
#define LWT_DIRENT_H
//------------------------------------------------------------------------------

#include "IOServer.h"

#include <cerrno>

#include <dirent.h>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * A wrapper for the opendir call to be performed in an I/O server.
 */
class OpenDir : public IOServer::ErrnoOperation
{
public:
    /**
     * Make a call with this operation.
     */
    static DIR* call(const char* name);

private:
    /**
     * The name of the directory to open.
     */
    const char* name;

    /**
     * The directory stream returned.
     */
    DIR* stream;

public:
    /**
     * Construct the operation.
     */
    OpenDir(const char* name);

    /**
     * Get the stream.
     */
    DIR* getStream() const;

protected:
    /**
     * Perform the operation in the worker thread.
     */
    virtual void performErrno();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * A wrapper for the readdir_r call to be performed in an I/O server.
 */
class ReadDir : public IOServer::ErrnoOperation
{
public:
    /**
     * Make a call with this operation.
     */
    static int call(DIR* dirp, struct dirent* entry, struct dirent** result);

private:
    /**
     * The directory stream to work on.
     */
    DIR* dirp;

    /**
     * The directory entry to put the result into. On return it will
     * be 0, if readdir_r returned successfully, but the end of the
     * directory has been reached, otherwise it will remain the entry
     * given.
     */
    struct dirent* entry;

    /**
     * The result code.
     */
    int result;

public:
    /**
     * Construct the operation.
     */
    ReadDir(DIR* dirp, struct dirent* entry);

    /**
     * Get the result.
     */
    int getResult() const;

    /**
     * Get the entry;
     */
    struct dirent* getEntry() const;

protected:
    /**
     * Perform the operation in the worker thread.
     */
    virtual void performErrno();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/**
 * A wrapper for the closedir call to be performed in an I/O server.
 */
class CloseDir : public IOServer::ErrnoOperation
{
public:
    /**
     * Make a call with this operation.
     */
    static int call(DIR* dirp);
    
private:
    /**
     * The directory stream to close.
     */
    DIR* dirp;

    /**
     * The result code.
     */
    int result;

public:
    /**
     * Construct the operation.
     */
    CloseDir(DIR* dirp);

    /**
     * Get the result.
     */
    int getResult() const;

protected:
    /**
     * Perform the operation in the worker thread.
     */
    virtual void performErrno();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Inline definitions
//------------------------------------------------------------------------------

inline OpenDir::OpenDir(const char* name) :
    name(name),
    stream(0)
{
}

//------------------------------------------------------------------------------

inline DIR* OpenDir::getStream() const
{
    return stream;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline ReadDir::ReadDir(DIR* dirp, struct dirent* entry) :
    dirp(dirp),
    entry(entry),
    result(-1)
{
}

//------------------------------------------------------------------------------

inline int ReadDir::getResult() const
{
    return result;
}

//------------------------------------------------------------------------------

inline struct dirent* ReadDir::getEntry() const
{
    return entry;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

inline CloseDir::CloseDir(DIR* dirp) :
    dirp(dirp),
    result(-1)
{
}

//------------------------------------------------------------------------------

inline int CloseDir::getResult() const
{
    return result;
}

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_DIRENT_H

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:

