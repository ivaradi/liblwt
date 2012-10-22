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

#include "Dirent.h"

//------------------------------------------------------------------------------

using lwt::OpenDir;
using lwt::ReadDir;
using lwt::CloseDir;

//------------------------------------------------------------------------------

DIR* OpenDir::call(const char* name)
{
    OpenDir operation(name);
    if (IOServer::get().execute(&operation)) {
        errno = operation.getErrorNumber();
        return operation.getStream();
    } else {
        return 0;
    }
}

//------------------------------------------------------------------------------

void OpenDir::performErrno()
{
    stream = opendir(name);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int ReadDir::call(DIR* dirp, struct dirent* entry, struct dirent** result)
{
    ReadDir operation(dirp, entry);
    if (IOServer::get().execute(&operation)) {
        errno = operation.getErrorNumber();
        *result = operation.getEntry();
        return operation.getResult();
    } else {
        return -1;
    }
}

//------------------------------------------------------------------------------

void ReadDir::performErrno()
{
    result = readdir_r(dirp, entry, &entry);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int CloseDir::call(DIR* dirp)
{
    CloseDir operation(dirp);
    if (IOServer::get().execute(&operation)) {
        errno = operation.getErrorNumber();
        return operation.getResult();
    } else {
        return -1;
    }
}

//------------------------------------------------------------------------------

void CloseDir::performErrno()
{
    result = closedir(dirp);
}

//------------------------------------------------------------------------------

// Local Variables:
// mode: C++
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:

