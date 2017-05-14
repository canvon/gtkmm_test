/*
 *  ls-gui-cvn - ls (list directory command) GUI by canvon
 *  Copyright (C) 2017  Fabian Pietsch <fabian@canvon.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TIME_CVN_HH
#define TIME_CVN_HH

#include <chrono>
#include <string>

// Forward-declare to avoid need of including C-style system headers here.
struct timespec;
struct tm;

namespace cvn
{
namespace system_time
{

using clock = std::chrono::system_clock;
using time_point = clock::time_point;


// Convert from low-level C struct timespec to C++ std::chrono::system_clock::time_point.
time_point from_timespec(const ::timespec &ts);

// Convert from C++ std::chrono::system_clock::time_point to low-level C struct timespec.
::timespec to_timespec(const time_point &tp);


// Convert from std::chrono::system_clock::time_point to std::string.
std::string to_string(const time_point &tp, bool localtime = true);

// Get low-level C struct tm, with Greenwich Mean Time.
::tm get_tm_gmtime(const time_point &tp);

// Get low-level C struct tm, with local time.
::tm get_tm_localtime(const time_point &tp);

}  // cvn::system_time
}  // cvn

#endif  // TIME_CVN_HH
