#include "lsdirent.hh"
#include <system_error>
#include <iomanip>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>


class LsDirent::impl
{
public:
	DIR     *dirp;  // directory stream pointer
	dirent  *entp;  // directory entry pointer
};

LsDirent::LsDirent(const char *pathname)
{
	if (!pathname)
		throw std::invalid_argument("LsDirent ctor: argument pathname is required");

	// Initialize smartpointer.
	pimpl = std::make_shared<LsDirent::impl>();

	if ((pimpl->dirp = opendir(pathname)) == nullptr) {
		std::ostringstream os;
		os << "LsDirent ctor: library function opendir() failed for " << std::quoted(pathname);
		throw std::system_error(errno, std::generic_category(), os.str());
	}
}

LsDirent::LsDirent(const std::string &pathname_str) :
	LsDirent(pathname_str.c_str())
{
}

LsDirent::~LsDirent()
{
	// Ensure directory stream (and associated file descriptor) does not leak.
	if (pimpl->dirp)
		close();
}

void LsDirent::close()
{
	if (!pimpl->dirp)
		throw std::logic_error("LsDirent close(): invalid operation: directory stream not open");

	if (closedir(pimpl->dirp)) {
		throw std::system_error(errno, std::generic_category(), 
			"LsDirent close(): library function closedir() failed");
	}

	pimpl->dirp = nullptr;
	pimpl->entp = nullptr;
}

int LsDirent::fd()
{
	if (!pimpl->dirp)
		throw std::logic_error("LsDirent fd(): invalid operation: directory stream not open");

	int fd = 0;
	if ((fd = dirfd(pimpl->dirp)) < 0) {
		throw std::system_error(errno, std::generic_category(),
			"LsDirent fd(): library function dirfd() failed");
	}

	return fd;
}

bool LsDirent::read()
{
	if (!pimpl->dirp)
		throw std::logic_error("LsDirent read(): invalid operation: directory stream not open");

	errno = 0;
	if ((pimpl->entp = readdir(pimpl->dirp)) == nullptr && errno) {
		throw std::system_error(errno, std::generic_category(),
			"LsDirent read(): library function readdir() failed");
	}

	return pimpl->entp != nullptr;
}

std::string LsDirent::get_name()
{
	if (!pimpl->dirp)
		throw std::logic_error("LsDirent get_name(): invalid operation: directory stream not open");

	if (!pimpl->entp)
		throw std::logic_error("LsDirent get_name(): invalid operation: no current directory entry");

	return pimpl->entp->d_name;
}
