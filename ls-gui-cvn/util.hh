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

#ifndef LS_UTILITY_HH
#define LS_UTILITY_HH

#include "stat-cvn.hh"

#include <string>

//namespace cvn::fs
namespace cvn { namespace fs
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

	std::string get_current_dir_name();

	std::string dirname(const std::string &pathname_str);
	std::string basename(const std::string &pathname_str, const std::string &extension_str = "");

	bool is_hidden(const std::string &pathname_str);
	bool is_hidden(int dirfd, const std::string &pathname_str);

	std::string expand_path(const std::string &pathname);

}  // cvn::fs
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6

#endif  // LS_UTILITY_HH
