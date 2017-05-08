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

#ifndef DIRENT_CVN_HH
#define DIRENT_CVN_HH

#include <memory>
#include <string>

//namespace cvn::fs
namespace cvn { namespace fs
{

	class Dirent
	{
	public:
		Dirent(const char *pathname);
		Dirent(const std::string &pathname_str);
		~Dirent();

		void close();

		int fd();

		// Returns true if a directory entry has been read,
		//         false if the end of the directory stream has been reached.
		// Other cases will likely throw an exception.
		bool read();

		std::string get_ent_name();

		enum class EntType {
			Unknown,
			Regular,
			Directory,
			Symlink,
			BlockDevice,
			CharacterDevice,
			NamedPipe,
			Fifo = NamedPipe,
			Socket,
		};
		EntType get_ent_type();

	protected:
		class impl;
		std::shared_ptr<impl> pimpl;
	};

}  // cvn::fs
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6

#endif  // DIRENT_CVN_HH
