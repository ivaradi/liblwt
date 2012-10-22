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

#ifndef PROXYMUX_UTIL_H
#define PROXYMUX_UTIL_H
//------------------------------------------------------------------------------

typedef unsigned long long millis_t;

//------------------------------------------------------------------------------

const millis_t INVALID_MILLIS = static_cast<millis_t>(-1);

//------------------------------------------------------------------------------

millis_t currentTimeMillis();

//------------------------------------------------------------------------------
#endif // PROXYMUX_UTIL_H

// Local variables:
// mode: c++
// End:
