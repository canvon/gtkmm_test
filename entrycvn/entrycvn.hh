/*
 *  entrycvn - show some GTK+ entry widget functionality
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

#ifndef ENTRY_CANVON_HH
#define ENTRY_CANVON_HH

#include <gtkmm/box.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>

class EntryCvn : public Gtk::Window
{
public:
	EntryCvn();
	virtual ~EntryCvn();

protected:
	void on_editable_toggled();
	void on_visibility_toggled();
	void on_close_clicked();

	Gtk::Entry entry_;
	Gtk::Box outerBox_, middleInnerBox_;
	Gtk::CheckButton editableCheckButton_, visibilityCheckButton_;
	Gtk::Button closeButton_;
};

#endif  // ENTRY_CANVON_HH
