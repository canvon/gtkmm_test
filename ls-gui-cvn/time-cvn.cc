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

#include "time-cvn.hh"

#include <system_error>

#include <time.h>
#include <string.h>

namespace cvn
{
namespace system_time
{

time_point from_timespec(const ::timespec &ts)
{
	return
		time_point(
			std::chrono::seconds(ts.tv_sec) +
			std::chrono::nanoseconds(ts.tv_nsec)
		);
}

::timespec to_timespec(const time_point &tp)
{
	const clock::duration &tp_dur(tp.time_since_epoch());
	return
		{
			std::chrono::duration_cast<std::chrono::seconds>(tp_dur).count(),
			std::chrono::duration_cast<std::chrono::nanoseconds>(tp_dur).count()
		};
}


std::string to_string(const time_point &tp, bool localtime)
{
	::tm brokenDown = localtime ?
		get_tm_localtime(tp) :
		get_tm_gmtime(tp);
	char buf[256];

	if (strftime(buf, sizeof buf, "%c", &brokenDown) <= 0)
		throw std::runtime_error(__func__ + std::string(": couldn't format time"));

	return buf;
}


::tm get_tm_gmtime(const time_point &tp)
{
	time_t tt(clock::to_time_t(tp));

	::tm brokenDown;
	::memset(&brokenDown, '\0', sizeof brokenDown);
	brokenDown.tm_isdst = -1;

	if (::gmtime_r(&tt, &brokenDown) == nullptr)
		throw std::system_error(errno, std::generic_category(),
			__func__ + std::string(": library function gmtime_r() failed"));

	return brokenDown;
}

::tm get_tm_localtime(const time_point &tp)
{
	time_t tt(clock::to_time_t(tp));

	::tm brokenDown;
	::memset(&brokenDown, '\0', sizeof brokenDown);
	brokenDown.tm_isdst = -1;

	if (::localtime_r(&tt, &brokenDown) == nullptr)
		throw std::system_error(errno, std::generic_category(),
			__func__ + std::string(": library function localtime_r() failed"));

	return brokenDown;
}

}  // cvn::system_time
}  // cvn
