#include "util.hh"

#include <iomanip>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>


//
// cvn::readlink() family of functions
//


// From path name, as C or C++ string.

std::string cvn::readlink(const char *pathname)
{
	return readlink(pathname, LsLstat(pathname));
}

std::string cvn::readlink(const std::string &pathname_str)
{
	return readlink(pathname_str.c_str());
}


// From path name and stat result, as C or C++ string.

std::string cvn::readlink(const char *pathname, const LsStat &symlink_stat)
{
	return readlink(pathname, static_cast<int>(symlink_stat.get_size()));
}

std::string cvn::readlink(const std::string &pathname_str, const LsStat &symlink_stat)
{
	return readlink(pathname_str.c_str(), symlink_stat);
}


// From path name and expected size, as C or C++ string.
//
// This is implemented using readlinkat() now, to keep code duplication down.

std::string cvn::readlink(const char *pathname, int expected_size)
{
	return readlinkat(AT_FDCWD, pathname, expected_size);
}

std::string cvn::readlink(const std::string &pathname_str, int expected_size)
{
	return readlink(pathname_str.c_str(), expected_size);
}


//
// cvn::readlinkat() family of functions
//


// From directory file descriptor and path name, as C or C++ string.

std::string cvn::readlinkat(int dirfd, const char *pathname)
{
	return readlinkat(dirfd, pathname, LsLstat(pathname));
}

std::string cvn::readlinkat(int dirfd, const std::string &pathname_str)
{
	return readlinkat(dirfd, pathname_str.c_str());
}


// From directory file descriptor, path name and stat result, as C or C++ string.

std::string cvn::readlinkat(int dirfd, const char *pathname, const LsStat &symlink_stat)
{
	return readlinkat(dirfd, pathname, static_cast<int>(symlink_stat.get_size()));
}

std::string cvn::readlinkat(int dirfd, const std::string &pathname_str, const LsStat &symlink_stat)
{
	return readlinkat(dirfd, pathname_str.c_str(), symlink_stat);
}


// From directory file descriptor, path name and expected size, as C or C++ string.
//
// This is the "real" implementation, for now.

std::string cvn::readlinkat(int dirfd, const char *pathname, int expected_size)
{
	int symlink_size = expected_size;

	// Increase expected symlink size so that multiplying it will
	// increase the result we're prepared for significantly.
	if (symlink_size < 255)  // (Hard-coded to avoid use of PATH_MAX, which could impair portability to e.g. GNU Hurd?)
		symlink_size = 255;

	// Three total attempts, so possibly two attempts to avoid truncation.
	const int attempts_max = 3;
	for (int attempts_left = attempts_max; attempts_left > 0; attempts_left--)
	{
		char buf[symlink_size + 1];
		size_t  buf_size = sizeof buf / sizeof *buf;
		ssize_t ret;

		if ((ret = ::readlinkat(dirfd, pathname, buf, buf_size)) < 0) {
			std::ostringstream os;
			os << "cvn::readlinkat(): syscall readlinkat() failed"
			   << " at directory file descriptor " << dirfd
			   << " for " << std::quoted(pathname)
			   << " (with buffer size " << buf_size << ")";
			if (errno)
				os << ": " << strerror(errno);
			throw std::runtime_error(os.str());
		}

		// No truncation, for sure?
		if (static_cast<size_t>(ret) < buf_size) {
			buf[ret] = '\0';
			return std::string(buf);
		}

		//symlink_size *= 2;
		symlink_size = (symlink_size + 1) * 2 - 1;
	}

	// Couldn't avoid truncation.
	std::ostringstream os;
	os << "cvn::readlinkat(): couldn't avoid truncation after "
	   << attempts_max << " attempts";
	os << " (next expected symlink size would have been "
	   << symlink_size << ")";
	throw std::runtime_error(os.str());
}

std::string cvn::readlinkat(int dirfd, const std::string &pathname_str, int expected_size)
{
	return readlinkat(dirfd, pathname_str.c_str(), expected_size);
}
