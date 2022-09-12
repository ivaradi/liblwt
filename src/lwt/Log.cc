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

#include "Log.h"
#include "Thread.h"

#include <ctime>

//------------------------------------------------------------------------------

using lwt::Log;

using std::string;

//------------------------------------------------------------------------------

bool Log::enableStdOut = false;

//------------------------------------------------------------------------------

string Log::logFileName;

//------------------------------------------------------------------------------

void Log::log(bool error, const char* format, va_list& ap)
{
    char buf[1024];    
    size_t offset = 0;

    time_t t = time(0);
    offset += strftime(buf, sizeof(buf), "%F %T: ", localtime(&t));

    Thread* current = Thread::getCurrent();
    if (current!=0) {
        offset += snprintf(buf + offset, sizeof(buf) - offset, 
                           "%s: ", current->logContext.c_str());
    }
    vsnprintf(buf + offset, sizeof(buf) - offset, format, ap);

    output(error, buf);
}

//------------------------------------------------------------------------------

void Log::cont(bool error, const char* format, va_list& ap)
{
    char buf[1024];    
    vsnprintf(buf, sizeof(buf), format, ap);

    output(error, buf);
}


//------------------------------------------------------------------------------

void Log::output(bool error, const char* str)
{
    if (enableStdOut) {
        fprintf(error ? stderr : stdout, "%s", str);
    }
    if (!logFileName.empty()) {
        FILE* f = fopen(logFileName.c_str(), "at");
        fprintf(f, "%s", str);
        fclose(f);
    }
}

//------------------------------------------------------------------------------
