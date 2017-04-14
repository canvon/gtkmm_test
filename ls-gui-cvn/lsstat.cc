#include "lsstat.hh"
#include <stdexcept>
#include <iomanip>

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

	// Initialize smartpointer.
	pimpl = std::make_shared<LsStat::impl>();

	if (stat(pathname, &pimpl->sb)) {
		std::ostringstream os;
		os << "LsStat ctor: syscall stat() failed for " << std::quoted(pathname);
		if (errno)
			os << ": " << strerror(errno);
		throw std::runtime_error(os.str());
	}
}

struct stat &LsStat::get_stat()
{
	return pimpl->sb;
}
