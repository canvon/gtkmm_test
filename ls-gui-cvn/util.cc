#include "util.hh"
#include <system_error>
#include <sstream>
#include <iomanip>

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <cerrno>

//namespace cvn::fs
namespace cvn { namespace fs
{


	//
	// cvn::fs::readlink() family of functions
	//


	// From path name, as C or C++ string.

	std::string readlink(const char *pathname)
	{
		return readlink(pathname, Lstat(pathname));
	}

	std::string readlink(const std::string &pathname_str)
	{
		return readlink(pathname_str.c_str());
	}


	// From path name and stat result, as C or C++ string.

	std::string readlink(const char *pathname, const Stat &symlink_stat)
	{
		return readlink(pathname, static_cast<int>(symlink_stat.get_size()));
	}

	std::string readlink(const std::string &pathname_str, const Stat &symlink_stat)
	{
		return readlink(pathname_str.c_str(), symlink_stat);
	}


	// From path name and expected size, as C or C++ string.
	//
	// This is implemented using readlinkat() now, to keep code duplication down.

	std::string readlink(const char *pathname, int expected_size)
	{
		return readlinkat(AT_FDCWD, pathname, expected_size);
	}

	std::string readlink(const std::string &pathname_str, int expected_size)
	{
		return readlink(pathname_str.c_str(), expected_size);
	}


	//
	// cvn::fs::readlinkat() family of functions
	//


	// From directory file descriptor and path name, as C or C++ string.

	std::string readlinkat(int dirfd, const char *pathname)
	{
		return readlinkat(dirfd, pathname, Lstat(pathname));
	}

	std::string readlinkat(int dirfd, const std::string &pathname_str)
	{
		return readlinkat(dirfd, pathname_str.c_str());
	}


	// From directory file descriptor, path name and stat result, as C or C++ string.

	std::string readlinkat(int dirfd, const char *pathname, const Stat &symlink_stat)
	{
		return readlinkat(dirfd, pathname, static_cast<int>(symlink_stat.get_size()));
	}

	std::string readlinkat(int dirfd, const std::string &pathname_str, const Stat &symlink_stat)
	{
		return readlinkat(dirfd, pathname_str.c_str(), symlink_stat);
	}


	// From directory file descriptor, path name and expected size, as C or C++ string.
	//
	// This is the "real" implementation, for now.

	std::string readlinkat(int dirfd, const char *pathname, int expected_size)
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
			// (This does not work on Debian 8 'jessie'
			// with g++ 4.9.2, as it compiles the sizeof of a
			// variable-length array to constant 1...
			// With Debian testing 'stretch' g++ 6.3.0,
			// it just worked.)
			//char    buf[symlink_size + 1];
			//size_t  buf_size = sizeof buf;
			size_t  buf_size = symlink_size + 1;
			char    buf[buf_size];
			ssize_t ret;

			if ((ret = ::readlinkat(dirfd, pathname, buf, buf_size)) < 0) {
				std::ostringstream os;
				os << "cvn::fs::readlinkat(): syscall readlinkat() failed"
				   << " at directory file descriptor " << dirfd
				   << " for " << std::quoted(pathname)
				   << " (with buffer size " << buf_size << ")";
				throw std::system_error(errno, std::generic_category(), os.str());
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
		os << "cvn::fs::readlinkat(): couldn't avoid truncation after "
		   << attempts_max << " attempts";
		os << " (next expected symlink size would have been "
		   << symlink_size << ")";
		os << ", at directory file descriptor " << dirfd
		   << " and pathname " << std::quoted(pathname);
		throw std::runtime_error(os.str());
	}

	std::string readlinkat(int dirfd, const std::string &pathname_str, int expected_size)
	{
		return readlinkat(dirfd, pathname_str.c_str(), expected_size);
	}


	std::string get_current_dir_name()
	{
		char *dir_name_ptr = nullptr;
		bool  dir_name_malloced = false;

		// Make portable by not relying on GNU extension
		// on platforms which don't support it.
#ifdef __GLIBC__
		dir_name_ptr = ::get_current_dir_name();
		if (dir_name_ptr)
			dir_name_malloced = true;
#else
		for (size_t buf_size = 4096; buf_size <= 4*1024*1024; buf_size *= 2)
		{
			char  buf[buf_size];
			errno = 0;
			dir_name_ptr = ::getcwd(buf, buf_size);
			if (dir_name_ptr || errno != ERANGE)
				break;
		}
#endif

		if (!dir_name_ptr) {
			throw std::system_error(errno, std::generic_category(),
				"cannot get current directory name");
		}

		std::string dir_name(dir_name_ptr);
		if (dir_name_malloced && dir_name_ptr) {
			free(dir_name_ptr);
			dir_name_ptr = nullptr;
		}
		return dir_name;
	}


	//
	// dirname(), basename() & is_hidden()
	//

	std::string dirname(const std::string &pathname_str)
	{
		// Find right-most slash.
		std::string::size_type slash_pos = pathname_str.rfind('/');
		// No slash found means filename-only; assume current directory.
		if (slash_pos == std::string::npos || slash_pos < 0)
			return ".";
		// Root directory should keep its slash.
		if (slash_pos == 0)
			return "/";

		// Remove everything from the right-most slash onwards.
		return pathname_str.substr(0, slash_pos);
	}

	std::string basename(const std::string &pathname_str, const std::string &extension_str)
	{
		std::string bn_with_ext("");
		std::string::size_type slash_pos = pathname_str.rfind('/');
		if (slash_pos == std::string::npos || slash_pos < 0)
			bn_with_ext = pathname_str;
		else {
			if (slash_pos + 1 < pathname_str.length())
				bn_with_ext = pathname_str.substr(slash_pos + 1);
		}

		if (extension_str.empty() || extension_str.length() >= bn_with_ext.length())
			return bn_with_ext;

		std::string::size_type ext_start = bn_with_ext.length() - extension_str.length();
		if (bn_with_ext.substr(ext_start) == extension_str)
			return bn_with_ext.substr(0, ext_start);

		return bn_with_ext;
	}

	bool is_hidden(const std::string &pathname_str)
	{
		std::string bn = basename(pathname_str);

		// On UNIX-like, a hidden file/directory is just a dot-file /
		// starts with a dot '.'
		return !bn.empty() && bn[0] == '.';
	}
	
	bool is_hidden(int dirfd, const std::string &pathname_str)
	{
		// As long as is_hidden(string) just compares the basename against .*,
		// we'll simply defer to it.
		return is_hidden(pathname_str);
	}


	std::string expand_path(const std::string &pathname)
	{
		if (pathname.empty())
			return pathname;

		std::string ret(pathname);

		if (ret[0] == '~') {
			if (ret.length() == 1 || ret[1] == '/') {
				const char *home = ::getenv("HOME");
				if (home == nullptr)
					throw std::runtime_error("cvn::fs::expand_path(): tilde expansion: "
						"environment variable ``HOME'' not set");

				ret.replace(0, 1, home);
			}
			else {
				std::string::size_type slashPos = ret.find('/'), tildeLen = 0;
				if (slashPos == std::string::npos) {
					tildeLen = ret.length();
				}
				else {
					tildeLen = slashPos;
				}
				if (tildeLen <= 1)
					throw std::runtime_error("cvn::fs::expand_path(): tilde expansion: "
						"invalid tilde length");

				std::string userName(ret.substr(1, tildeLen - 1));

				errno = 0;
				const ::passwd *user = ::getpwnam(userName.c_str());
				if (!user) {
					if (errno) {
						std::ostringstream os;
						os << "cvn::fs::expand_path(): tilde expansion: "
						   << "library function getpwnam() failed for user name "
						   << std::quoted(userName);
						throw std::system_error(errno, std::generic_category(), os.str());
					}
					else {
						std::ostringstream os;
						os << "cvn::fs::expand_path(): tilde expansion: "
						   << "user name " << std::quoted(userName) << " not found";
						throw std::runtime_error(os.str());
					}
				}

				ret.replace(0, tildeLen, user->pw_dir);
			}
		}

		return ret;
	}

}  // cvn::fs
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6
