/*
 *  ls-gui-cvn - ls (list directory command) GUI by canvon
 *  Copyright (C) 2017  Fabian Pietsch <fabian@canvon.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stat-cvn.hh"
#include <system_error>
#include <sstream>
#include <iomanip>

#include "../replacements/std_quoted.hh"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <errno.h>


//namespace cvn::fs
namespace cvn { namespace fs
{

	class Stat::impl
	{
	public:
		struct ::stat sb;  // stat buffer
	};

	// Basic constructor, to be used by Stat itself and all derived classes.
	Stat::Stat()
	{
		// Initialize smartpointer.
		pimpl = std::make_shared<Stat::impl>();
	}


	// Constructors for stat().

	Stat::Stat(const char *pathname) :
		Stat()
	{
		if (!pathname)
			throw std::invalid_argument("Stat ctor: argument pathname is required");

		if (stat(pathname, &pimpl->sb)) {
			std::ostringstream os;
			os << "Stat ctor: cannot retrieve stat information for " << std::quoted(pathname);
			throw std::system_error(errno, std::generic_category(), os.str());
		}
	}

	Stat::Stat(const std::string &pathname_str) :
		Stat(pathname_str.c_str())
	{
	}


	// Constructors for lstat() (instead of stat()).

	Lstat::Lstat(const char *pathname) :
		Stat()
	{
		if (!pathname)
			throw std::invalid_argument("Lstat ctor: argument pathname is required");

		if (lstat(pathname, &pimpl->sb)) {
			std::ostringstream os;
			os << "Lstat ctor: cannot retrieve lstat information for " << std::quoted(pathname);
			throw std::system_error(errno, std::generic_category(), os.str());
		}
	}

	Lstat::Lstat(const std::string &pathname_str) :
		Lstat(pathname_str.c_str())
	{
	}


	// Constructors for fstatat() (instead of stat()).

	Fstatat::Fstatat(int dirfd, const char *pathname, bool symlink_nofollow) :
		Stat()
	{
		// (Allow possible special-case.)
		//if (!pathname)
		//	throw std::invalid_argument("Fstatat ctor: argument pathname is required");

		int flags = 0;
		if (symlink_nofollow)
			flags |= AT_SYMLINK_NOFOLLOW;
		if (fstatat(dirfd, pathname, &pimpl->sb, flags)) {
			std::ostringstream os;
			os << "Fstatat ctor: cannot retrieve fstatat information at file descriptor "
			   << dirfd << " for " << std::quoted(pathname);
			if (flags)
				os << " (with flags " << flags << ")";
			throw std::system_error(errno, std::generic_category(), os.str());
		}
	}

	Fstatat::Fstatat(int dirfd, const std::string &pathname_str, bool symlink_nofollow) :
		Fstatat(dirfd, pathname_str.c_str(), symlink_nofollow)
	{
	}


	// The rest is shared between Stat and all derived classes.

	bool Stat::get_is_reg () const { return S_ISREG (pimpl->sb.st_mode); }
	bool Stat::get_is_dir () const { return S_ISDIR (pimpl->sb.st_mode); }
	bool Stat::get_is_chr () const { return S_ISCHR (pimpl->sb.st_mode); }
	bool Stat::get_is_blk () const { return S_ISBLK (pimpl->sb.st_mode); }
	bool Stat::get_is_fifo() const { return S_ISFIFO(pimpl->sb.st_mode); }
	bool Stat::get_is_lnk () const { return S_ISLNK (pimpl->sb.st_mode); }
	bool Stat::get_is_sock() const { return S_ISSOCK(pimpl->sb.st_mode); }

	bool Stat::get_filemode_set_uid() const  { return (pimpl->sb.st_mode & S_ISUID) == S_ISUID; }
	bool Stat::get_filemode_set_gid() const  { return (pimpl->sb.st_mode & S_ISGID) == S_ISGID; }
	bool Stat::get_filemode_sticky () const  { return (pimpl->sb.st_mode & S_ISVTX) == S_ISVTX; }
	bool Stat::get_filemode_r_user () const  { return (pimpl->sb.st_mode & S_IRUSR) == S_IRUSR; }
	bool Stat::get_filemode_w_user () const  { return (pimpl->sb.st_mode & S_IWUSR) == S_IWUSR; }
	bool Stat::get_filemode_x_user () const  { return (pimpl->sb.st_mode & S_IXUSR) == S_IXUSR; }
	bool Stat::get_filemode_r_group() const  { return (pimpl->sb.st_mode & S_IRGRP) == S_IRGRP; }
	bool Stat::get_filemode_w_group() const  { return (pimpl->sb.st_mode & S_IWGRP) == S_IWGRP; }
	bool Stat::get_filemode_x_group() const  { return (pimpl->sb.st_mode & S_IXGRP) == S_IXGRP; }
	bool Stat::get_filemode_r_other() const  { return (pimpl->sb.st_mode & S_IROTH) == S_IROTH; }
	bool Stat::get_filemode_w_other() const  { return (pimpl->sb.st_mode & S_IWOTH) == S_IWOTH; }
	bool Stat::get_filemode_x_other() const  { return (pimpl->sb.st_mode & S_IXOTH) == S_IXOTH; }

	int Stat::get_filemode_rwx_user () const { return pimpl->sb.st_mode & S_IRWXU; }
	int Stat::get_filemode_rwx_group() const { return pimpl->sb.st_mode & S_IRWXG; }
	int Stat::get_filemode_rwx_other() const { return pimpl->sb.st_mode & S_IRWXO; }

	std::string Stat::get_mode_str() const
	{
		std::ostringstream os;


		// file type

		if      (get_is_reg())   os << '-';
		else if (get_is_dir())   os << 'd';
		else if (get_is_chr())   os << 'c';
		else if (get_is_blk())   os << 'b';
		else if (get_is_fifo())  os << 'p';
		else if (get_is_lnk())   os << 'l';
		else if (get_is_sock())  os << 's';
		else                     os << '?';


		// user perms

		if      (get_filemode_r_user())  os << 'r';
		else                             os << '-';

		if      (get_filemode_w_user())  os << 'w';
		else                             os << '-';

		if (get_filemode_set_uid()) {
			if      (get_filemode_x_user())  os << 's';
			else                             os << 'S';
		}
		else {
			if      (get_filemode_x_user())  os << 'x';
			else                             os << '-';
		}


		// group perms

		if      (get_filemode_r_group())  os << 'r';
		else                              os << '-';

		if      (get_filemode_w_group())  os << 'w';
		else                              os << '-';

		if (get_filemode_set_gid()) {
			if      (get_filemode_x_group())  os << 's';
			else                              os << 'S';
		}
		else {
			if      (get_filemode_x_group())  os << 'x';
			else                              os << '-';
		}


		// other perms

		if      (get_filemode_r_other())  os << 'r';
		else                              os << '-';

		if      (get_filemode_w_other())  os << 'w';
		else                              os << '-';

		if (get_filemode_sticky()) {
			if      (get_filemode_x_other())  os << 't';
			else                              os << 'T';
		}
		else {
			if      (get_filemode_x_other())  os << 'x';
			else                              os << '-';
		}


		return os.str();
	}

	int Stat::get_nlink() const { return pimpl->sb.st_nlink; }

	int Stat::get_uid  () const { return pimpl->sb.st_uid;   }
	int Stat::get_gid  () const { return pimpl->sb.st_gid;   }

	std::string Stat::get_user() const
	{
		uid_t uid = pimpl->sb.st_uid;

		// TODO: Use reentrant getpwuid_r(), to support threading?
		errno = 0;
		const ::passwd *user = ::getpwuid(uid);
		if (!user && errno)
			throw std::system_error(
				errno, std::generic_category(),
				"Stat::get_user(): library function getpwuid() failed for uid "
				+ std::to_string(uid));
		if (user)
			return user->pw_name;

		// Fall back to uid stringification.
		return std::to_string(uid);
	}

	std::string Stat::get_group() const
	{
		gid_t gid = pimpl->sb.st_gid;

		// TODO: Use reentrant getgrgid_r(), to support threading?
		errno = 0;
		const ::group *group = ::getgrgid(gid);
		if (!group && errno)
			throw std::system_error(
				errno, std::generic_category(),
				"Stat::get_group(): library function getgrgid() failed for gid "
				+ std::to_string(gid));
		if (group)
			return group->gr_name;

		// Fall back to gid stringification.
		return std::to_string(gid);
	}

	long long Stat::get_size() const
	{
		return pimpl->sb.st_size;
	}

// Sub-second precision file timestamps are system-specific / non-portable.
#if __linux__
	Time Stat::get_mtime() const { return pimpl->sb.st_mtim; }
	Time Stat::get_ctime() const { return pimpl->sb.st_ctim; }
	Time Stat::get_atime() const { return pimpl->sb.st_atim; }
#elif __APPLE__
	Time Stat::get_mtime() const { return pimpl->sb.st_mtimespec; }
	Time Stat::get_ctime() const { return pimpl->sb.st_ctimespec; }
	Time Stat::get_atime() const { return pimpl->sb.st_atimespec; }
#else
#pragma message("Warning: No sub-second precision in file timestamps available on this platform!")
	// This is expected to initialize the cvn::Time via conversion
	// from time_t to cvn::Time::seconds_type.
	Time Stat::get_mtime() const { return pimpl->sb.st_mtime; }
	Time Stat::get_ctime() const { return pimpl->sb.st_ctime; }
	Time Stat::get_atime() const { return pimpl->sb.st_atime; }
#endif

	struct ::stat &Stat::get_stat()
	{
		return pimpl->sb;
	}

	const struct ::stat &Stat::get_stat() const
	{
		return pimpl->sb;
	}

}  // cvn::fs
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6
