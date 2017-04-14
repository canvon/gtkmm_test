#include "lsstat.hh"
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


class LsStat::impl
{
public:
	struct stat sb;  // stat buffer
};

LsStat::LsStat(const char *pathname)
{
	if (!pathname)
		throw std::invalid_argument("LsStat ctor: argument pathname is required");

	if (stat(pathname, &pimpl->sb))
		throw std::runtime_error(
			std::string("LsStat ctor: syscall stat() failed: ")
			+ strerror(errno));
}

struct stat &LsStat::get_stat()
{
	return pimpl->sb;
}
