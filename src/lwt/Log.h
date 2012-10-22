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

#ifndef LWT_LOG_H
#define LWT_LOG_H
//------------------------------------------------------------------------------

#include <string>

#include <cstdio>
#include <cstdarg>

//------------------------------------------------------------------------------

namespace lwt {

//------------------------------------------------------------------------------

/**
 * Class to log stuff. It precedes each message by the context of the
 * actual thread, if there is an actual thread.
 */
class Log
{
public:
    /**
     * Indicate if the standard output/error are enabled.
     */
    static bool enableStdOut;
    
    /**
     * The name of a file to log into. If empty, logging will not be
     * done into a file
     */
    static std::string logFileName;

    /**
     * Log something
     */
    static void log(bool error, const char* format, va_list& ap);

    /**
     * Continue a log entry, i.e. do not print the prefix
     */
    static void cont(bool error, const char* format, va_list& ap);

private:
    /**
     * Output the given string into all log destinations
     */
    static void output(bool error, const char* str);
};

//------------------------------------------------------------------------------

} /* namespace lwt */

//------------------------------------------------------------------------------
#endif // LWT_LOG_H

// Local variables:
// mode: c++
// End:
