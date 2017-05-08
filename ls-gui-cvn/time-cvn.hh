#ifndef TIME_CVN_HH
#define TIME_CVN_HH

#include <memory>
#include <string>

// Forward-declare to avoid need of including C-style system headers here.
struct timespec;
struct tm;

namespace cvn
{

	class Time
	{
	public:
		typedef long long seconds_type;  // Seconds: Ensure 64bits resolution. (?)
		typedef      long nanosecs_type;  // Nanoseconds: 32bits would suffice,
			// but make processing easier on 64bit machines. (?)
			// It is expected that two 64bit registers would be used
			// to store a seconds:nanoseconds combination.
			// Whereas 32bit machines would be using three registers,
			// two to store the seconds and one for the nanoseconds?
			// ...

		Time();
		Time(seconds_type unixTime, nanosecs_type nanosecs = 0);
		Time(const ::timespec &ts);
		Time(const Time &toCopy);
		~Time();

		Time &operator=(const Time &rhs);

		bool operator<(const Time &rhs) const;

		std::string str() const;

		::tm get_tm_gmtime() const;
		::tm get_tm_localtime() const;

		::timespec &get_timespec();
		const ::timespec &get_timespec() const;

	protected:
		class impl;
		std::unique_ptr<impl> pimpl;
	};

}

#endif  // TIME_CVN_HH
