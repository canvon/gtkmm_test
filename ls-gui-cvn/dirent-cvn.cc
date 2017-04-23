#include "dirent-cvn.hh"
#include <system_error>
#include <iomanip>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>


namespace cvn::fs
{

	class Dirent::impl
	{
	public:
		::DIR    *dirp;  // directory stream pointer
		::dirent *entp;  // directory entry pointer
	};

	Dirent::Dirent(const char *pathname)
	{
		if (!pathname)
			throw std::invalid_argument("Dirent ctor: argument pathname is required");

		// Initialize smartpointer.
		pimpl = std::make_shared<Dirent::impl>();

		if ((pimpl->dirp = opendir(pathname)) == nullptr) {
			std::ostringstream os;
			os << "Dirent ctor: library function opendir() failed for " << std::quoted(pathname);
			throw std::system_error(errno, std::generic_category(), os.str());
		}
	}

	Dirent::Dirent(const std::string &pathname_str) :
		Dirent(pathname_str.c_str())
	{
	}

	Dirent::~Dirent()
	{
		// Ensure directory stream (and associated file descriptor) does not leak.
		if (pimpl->dirp)
			close();
	}

	void Dirent::close()
	{
		if (!pimpl->dirp)
			throw std::logic_error("Dirent close(): invalid operation: directory stream not open");

		if (closedir(pimpl->dirp)) {
			throw std::system_error(errno, std::generic_category(), 
				"Dirent close(): library function closedir() failed");
		}

		pimpl->dirp = nullptr;
		pimpl->entp = nullptr;
	}

	int Dirent::fd()
	{
		if (!pimpl->dirp)
			throw std::logic_error("Dirent fd(): invalid operation: directory stream not open");

		int fd = 0;
		if ((fd = dirfd(pimpl->dirp)) < 0) {
			throw std::system_error(errno, std::generic_category(),
				"Dirent fd(): library function dirfd() failed");
		}

		return fd;
	}

	bool Dirent::read()
	{
		if (!pimpl->dirp)
			throw std::logic_error("Dirent read(): invalid operation: directory stream not open");

		errno = 0;
		if ((pimpl->entp = readdir(pimpl->dirp)) == nullptr && errno) {
			throw std::system_error(errno, std::generic_category(),
				"Dirent read(): library function readdir() failed");
		}

		return pimpl->entp != nullptr;
	}

	std::string Dirent::get_ent_name()
	{
		if (!pimpl->dirp)
			throw std::logic_error("Dirent get_ent_name(): invalid operation: directory stream not open");

		if (!pimpl->entp)
			throw std::logic_error("Dirent get_ent_name(): invalid operation: no current directory entry");

		return pimpl->entp->d_name;
	}

}
