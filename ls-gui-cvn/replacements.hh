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

#ifndef REPLACEMENTS_HH
#define REPLACEMENTS_HH

#include "config.h"

#if !HAVE_STD_QUOTED
#pragma message("Using replacement for std::quoted...")
namespace std {

std::string quoted(const std::string &str)
{
	std::string out("\"");
	for (std::string::const_iterator iter(str.begin()); iter != str.end(); iter++)
	{
		if (*iter == '"')
			out += "\\\"";
		else
			out += *iter;
	}
	out += "\"";
	return out;
}

}
#endif

#endif  // REPLACEMENTS_HH
