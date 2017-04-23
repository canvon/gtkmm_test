#ifndef LS_UTILITY_HH
#define LS_UTILITY_HH

#include "stat-cvn.hh"

#include <string>

namespace cvn::fs
{
	std::string readlink(const char *pathname);
	std::string readlink(const std::string &pathname_str);
	std::string readlink(const char *pathname, const Stat &symlink_stat);
	std::string readlink(const std::string &pathname_str, const Stat &symlink_stat);
	std::string readlink(const char *pathname, int expected_size);
	std::string readlink(const std::string &pathname_str, int expected_size);

	std::string readlinkat(int dirfd, const char *pathname);
	std::string readlinkat(int dirfd, const std::string &pathname_str);
	std::string readlinkat(int dirfd, const char *pathname, const Stat &symlink_stat);
	std::string readlinkat(int dirfd, const std::string &pathname_str, const Stat &symlink_stat);
	std::string readlinkat(int dirfd, const char *pathname, int expected_size);
	std::string readlinkat(int dirfd, const std::string &pathname_str, int expected_size);

	std::string dirname(const std::string &pathname_str);
	std::string basename(const std::string &pathname_str, const std::string &extension_str = "");

	bool is_hidden(const std::string &pathname_str);
	bool is_hidden(int dirfd, const std::string &pathname_str);
}

#endif  // LS_UTILITY_HH
