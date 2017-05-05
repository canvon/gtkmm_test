#include "time-cvn.hh"

#include <system_error>

#include <time.h>
#include <string.h>

namespace cvn
{

	class Time::impl
	{
	public:
		::timespec ts;

		impl()
		{
			ts = { 0, 0 };
		}

		impl(seconds_type unixTime, nanosecs_type nanosecs)
		{
			ts.tv_sec  = unixTime;
			ts.tv_nsec = nanosecs;
		}

		impl(const ::timespec &ts)
		{
			this->ts = ts;
		}
	};


	Time::Time()
	{
		pimpl = std::make_unique<impl>();
	}

	Time::Time(seconds_type unixTime, nanosecs_type nanosecs)
	{
		pimpl = std::make_unique<impl>(unixTime, nanosecs);
	}

	Time::Time(const ::timespec &ts)
	{
		pimpl = std::make_unique<impl>(ts);
	}

	Time::Time(const Time &toCopy)
	{
		pimpl = std::make_unique<impl>(*toCopy.pimpl);
	}

	Time::~Time()
	{
	}


	Time &Time::operator=(const Time &rhs)
	{
		*pimpl = *rhs.pimpl;
		return *this;
	}


	std::string Time::str()
	{
		::tm brokenDown = get_tm_localtime();
		char buf[256];

		if (strftime(buf, sizeof buf, "%c", &brokenDown) <= 0)
			throw std::runtime_error("Time: couldn't format time");

		return buf;
	}


	::tm Time::get_tm_gmtime() const
	{
		::tm brokenDown;
		::memset(&brokenDown, '\0', sizeof brokenDown);
		brokenDown.tm_isdst = -1;

		if (::gmtime_r(&pimpl->ts.tv_sec, &brokenDown) == nullptr)
			throw std::system_error(errno, std::generic_category(),
				"Time: library function gmtime_r() failed");

		return brokenDown;
	}

	::tm Time::get_tm_localtime() const
	{
		::tm brokenDown;
		::memset(&brokenDown, '\0', sizeof brokenDown);
		brokenDown.tm_isdst = -1;

		if (::localtime_r(&pimpl->ts.tv_sec, &brokenDown) == nullptr)
			throw std::system_error(errno, std::generic_category(),
				"Time: library function localtime_r() failed");

		return brokenDown;
	}


	::timespec &Time::get_timespec()
	{
		return pimpl->ts;
	}

	const ::timespec &Time::get_timespec() const
	{
		return pimpl->ts;
	}

}  // cvn
