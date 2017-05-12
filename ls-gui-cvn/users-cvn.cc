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

#include "users-cvn.hh"
#include <system_error>
#include <iomanip>

#include "../replacements/std_make_unique.hh"

#include <sys/types.h>
#include <pwd.h>
#include <errno.h>


namespace cvn
{

	class Users::impl
	{
	public:
		::passwd *entp;  // passwd entry pointer
		bool closed;

		impl() : entp(nullptr), closed(false)
		{
		}

		void haveEntryOrThrow(const std::string &msg_prefix)
		{
			if (entp)
				return;

			throw std::logic_error(msg_prefix
				+ ": invalid operation: there is no current password database entry");
		}
	};

	Users::Users()
	{
		// Initialize smartpointer.
		pimpl = std::make_unique<impl>();
	}

	Users::~Users()
	{
		if (!pimpl->closed)
			close();
	}

	void Users::close()
	{
		if (pimpl->closed)
			throw std::logic_error("Users close(): invalid operation: "
				"password database has already been closed");

		endpwent();
		pimpl->closed = true;
		pimpl->entp = nullptr;
	}

	void Users::rewind()
	{
		pimpl->entp = nullptr;

		setpwent();
		pimpl->closed = false;
	}

	bool Users::read()
	{
		if (pimpl->closed)
			throw std::logic_error("Users read(): invalid operation: "
				"password database is closed");

		errno = 0;
		if ((pimpl->entp = getpwent()) == nullptr && errno)
			throw std::system_error(errno, std::generic_category(),
				"Users read(): library function getpwent() failed");

		return pimpl->entp != nullptr;
	}

	std::string Users::get_ent_username()
	{
		pimpl->haveEntryOrThrow("Users get entry username");
		return pimpl->entp->pw_name;
	}

	std::string Users::get_ent_password()
	{
		pimpl->haveEntryOrThrow("Users get entry password");
		return pimpl->entp->pw_passwd;
	}

	int Users::get_ent_uid()
	{
		pimpl->haveEntryOrThrow("Users get entry UID");
		return pimpl->entp->pw_uid;
	}

	int Users::get_ent_gid()
	{
		pimpl->haveEntryOrThrow("Users get entry GID");
		return pimpl->entp->pw_gid;
	}

	std::string Users::get_ent_gecos()
	{
		pimpl->haveEntryOrThrow("Users get entry GECOS");
		return pimpl->entp->pw_gecos;
	}

	std::string Users::get_ent_homedir()
	{
		pimpl->haveEntryOrThrow("Users get entry home directory");
		return pimpl->entp->pw_dir;
	}

	std::string Users::get_ent_shell()
	{
		pimpl->haveEntryOrThrow("Users get entry shell");
		return pimpl->entp->pw_shell;
	}

}  // cvn
