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

#ifndef STAT_CVN_HH
#define STAT_CVN_HH

#include "time-cvn.hh"

#include <memory>
#include <string>

// (Forward-declare so that we can return a reference to the stat buffer
// without needing to include the system headers in using code.)
struct stat;

//namespace cvn::fs
namespace cvn { namespace fs
{

	// A stat() and struct stat wrapper.
	class Stat
	{
	public:
		Stat(const char *pathname);
		Stat(const std::string &pathname_str);

		bool get_is_reg () const,
		     get_is_dir () const,
		     get_is_chr () const,
		     get_is_blk () const,
		     get_is_fifo() const,
		     get_is_lnk () const,
		     get_is_sock() const;
		bool get_filemode_set_uid() const,
		     get_filemode_set_gid() const,
		     get_filemode_sticky () const,
		     get_filemode_r_user () const,
		     get_filemode_w_user () const,
		     get_filemode_x_user () const,
		     get_filemode_r_group() const,
		     get_filemode_w_group() const,
		     get_filemode_x_group() const,
		     get_filemode_r_other() const,
		     get_filemode_w_other() const,
		     get_filemode_x_other() const;
		int get_filemode_rwx_user () const,
		    get_filemode_rwx_group() const,
		    get_filemode_rwx_other() const;
		std::string get_mode_str() const;

		int get_nlink() const;

		int          get_uid () const,  get_gid  () const;
		std::string  get_user() const,  get_group() const;

		long long get_size() const;

		Time get_mtime() const;
		Time get_ctime() const;
		Time get_atime() const;

		struct ::stat &get_stat();
		const struct ::stat &get_stat() const;

	protected:
		Stat();  // For use by derived classes.

		class impl;
		std::shared_ptr<impl> pimpl;
	};

	// Lstat retrieves information about the symbolic link instead of
	// the symlink target. (Uses lstat() internally instead of stat().)
	class Lstat : public Stat
	{
	public:
		Lstat(const char *pathname);
		Lstat(const std::string &pathname_str);
	};

	// Fstatat works from an already opened directory. (Uses fstatat().)
	class Fstatat : public Stat
	{
	public:
		Fstatat(int dirfd, const char *pathname, bool symlink_nofollow = false);
		Fstatat(int dirfd, const std::string &pathname_str, bool symlink_nofollow = false);
		Fstatat(const char *pathname) = delete;
		Fstatat(const std::string &pathname_str) = delete;
	};

}  // cvn::fs
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6

#endif  // STAT_CVN_HH
