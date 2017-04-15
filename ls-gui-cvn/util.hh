#ifndef LS_UTILITY_HH
#define LS_UTILITY_HH

#include "lsstat.hh"

#include <string>

namespace cvn
{
	std::string readlink(const char *pathname);
	std::string readlink(const std::string &pathname_str);
	std::string readlink(const char *pathname, const LsStat &symlink_stat);
	std::string readlink(const std::string &pathname_str, const LsStat &symlink_stat);
	std::string readlink(const char *pathname, int expected_size);
	std::string readlink(const std::string &pathname_str, int expected_size);
}

#endif  // LS_UTILITY_HH
