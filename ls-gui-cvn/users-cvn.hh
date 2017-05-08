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

#ifndef USERS_CVN_HH
#define USERS_CVN_HH

#include <memory>
#include <string>

namespace cvn
{

	class Users
	{
	public:
		Users();
		~Users();

		void close();
		void rewind();

		// Returns true if a password database entry has been read,
		//         false if the end of the password database has been reached.
		// Other cases will likely throw an exception.
		bool read();

		std::string get_ent_username();
		std::string get_ent_password();
		int get_ent_uid(), get_ent_gid();
		std::string get_ent_gecos();
		std::string get_ent_homedir();
		std::string get_ent_shell();

	protected:
		class impl;
		std::unique_ptr<impl> pimpl;
	};

}  // cvn

#endif  // USERS_CVN_HH
