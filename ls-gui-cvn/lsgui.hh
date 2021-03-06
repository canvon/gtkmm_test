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

#ifndef LS_GUI_HH
#define LS_GUI_HH

#include <list>
#include <map>
#include <vector>
#include "dirent-cvn.hh"
#include "time-cvn.hh"

#include <glib.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/infobar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodelsort.h>
#include <gtkmm/builder.h>
#include <gtkmm/menubar.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/menutoolbutton.h>
#include <giomm/menu.h>
#include <giomm/simpleaction.h>

#include "versioncheck.hh"


// (Forward-declare so we don't need to include the header from here.)
//namespace cvn::fs { class Stat; }
namespace cvn { namespace fs { class Stat; } }

//namespace cvn::lsgui
namespace cvn { namespace lsgui
{

	class LsGui : public Gtk::ApplicationWindow
	{
	public:
		LsGui();
		virtual ~LsGui();

		class LsModelColumns : public Gtk::TreeModel::ColumnRecord
		{
		public:
			Gtk::TreeModelColumn<Glib::ustring>
				perms;  // permissions
			Gtk::TreeModelColumn<guint>
				nlink;  // number of hard links
			Gtk::TreeModelColumn<Glib::ustring>
				user, group;
			Gtk::TreeModelColumn<guint>
				size;  // size in bytes
			Gtk::TreeModelColumn<cvn::system_time::time_point>
				time_lib;  // mtime, ctime or atime
			Gtk::TreeModelColumn<Glib::ustring>
				time_user;
			Gtk::TreeModelColumn<std::string>
				name_opsys;
			Gtk::TreeModelColumn<Glib::ustring>
				name_gui;
			Gtk::TreeModelColumn<Glib::ustring>
				name_user;
			Gtk::TreeModelColumn<cvn::fs::Dirent::EntType>
				type_lib;
			Gtk::TreeModelColumn<Glib::ustring>
				type_user;

			LsModelColumns();
		};

		LsModelColumns &get_modelColumns();
		Glib::RefPtr<Gtk::ListStore> get_model();

		void display_errmsg(const Glib::ustring &errmsg);
		void display_msg(const Glib::ustring &msg);
#ifndef HAVE_STRUCTURED_LOGGING
#pragma message("Warning: Omitting parameter \"fields\" from LsGui::display_glib_msg() declaration, " REQUIRES_STRUCTURED_LOGGING)
#endif
		void display_glib_msg(
			const Glib::ustring &log_domain,
			GLogLevelFlags log_level,
			const Glib::ustring &msg
#ifdef HAVE_STRUCTURED_LOGGING
			,
			const GLogField *fields = nullptr
#endif
			);

		bool history_is_valid() const;
		bool history_can_backward() const;
		bool history_can_forward() const;

		void history_clear();
		void history_add(const std::string &new_item_str);

		std::string get_location_str() const;
		bool get_location_is_dirlisting() const;
		void set_location_str();
		void set_location_str(const std::string &new_location_str);
		void set_location_str_relative(const std::string &rel_path);

		void fill_row(Gtk::TreeModel::Row &row,
			const int *dirfdptr,
			const std::string &name,
			const cvn::fs::Stat &name_stat);

		static const char *menubar_markup;

		Glib::RefPtr<Gio::MenuModel>  get_gmenu();
		Gtk::MenuBar                 *get_menubar_gtk();
		Gtk::Toolbar                 *get_toolbar();

		bool get_show_hidden() const;
		bool get_complete_location() const;

	protected:
		void set_title_addition(
			const Glib::ustring &title_addition = "",
			const Glib::ustring &short_name = "");

		void update_actions();
		void update_errorsInfoBar();
		void update_locationCompletion();
		void update_users();

		int on_model_sort_compare_time(
			const Gtk::TreeModel::iterator& a,
			const Gtk::TreeModel::iterator& b);

		void on_locationEntry_activate();
		void on_locationEntry_changed();
		bool on_locationEntry_key_press_event(GdkEventKey* key_event);
		void on_locationEntryCompletion_no_matches();
		void on_locationEntryCompletion_action_activated(int index);
		void on_errorsInfoBar_response(int response_id);
		void on_ls_row_activated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
		void on_ls_column_clicked(int columnNr);
		void on_action_open();
		void on_action_close();
		void on_action_reload();
		void on_action_backward();
		void on_action_forward();
		void on_action_up();
		void on_action_goto(const std::string &pathname);
		void on_action_show_hidden();
		void on_action_complete_location();

		LsModelColumns modelColumns_;
		Glib::RefPtr<Gtk::ListStore> model_;
		Glib::RefPtr<Gtk::TreeModelSort> modelSort_;

		std::string location_str_;
		bool location_is_dirlisting_;
		bool location_esc_pressed_;
		Glib::ustring::size_type locationEntry_lastSlashPos_;

		typedef std::list<std::string>   location_history_type;
		location_history_type            location_history_;
		location_history_type::iterator  location_history_pos_;


		enum class LocationCompletionAction {
			LoadUsers,
		};

		typedef std::vector<LocationCompletionAction>  locationCompletionActions_type;
		locationCompletionActions_type                 locationCompletionActions_;

		struct UsersCacheEntry {
			std::string username;
			std::string homedir;
			long uid;
		};
		typedef std::map<std::string, UsersCacheEntry>  usersCache_type;
		usersCache_type                                 usersCache_;

		void add_locationCompletionAction(
			LocationCompletionAction actionType, const Glib::ustring &actionText);
		void delete_locationCompletionActions(LocationCompletionAction actionType);


		struct ErrMsg
		{
			Glib::ustring   log_domain;
			GLogLevelFlags  log_level;
			Glib::ustring   msg;
		};
		typedef std::vector<ErrMsg> errmsgs_type;
		errmsgs_type errorMessages_lst_;

		Gtk::Box outerVBox_;
		Gtk::Box locationHBox_;
		Gtk::Label locationLabel_;
		Gtk::Entry location_;
		Glib::RefPtr<Gtk::ListStore> locationCompletionModel_ptr_;
		Glib::RefPtr<Gtk::EntryCompletion> locationCompletion_ptr_;
		Gtk::CellRendererText locationCompletionDirentTypeCellRenderer_;
		Gtk::InfoBar errorsInfoBar_; int posErrorsInfoBar_;
		Gtk::ScrolledWindow scrollErrorMessage_;
		Gtk::Label errorMessage_;
		Gtk::ScrolledWindow scrollLs_;
		Gtk::TreeView ls_;

		struct LsViewColumns
		{
			int perms, nlink, user, group, size, time, name;
		} lsViewColumns_;

		Glib::RefPtr<Gtk::Builder>  builder_ptr_;
		Glib::RefPtr<Gio::Menu>     gmenu_ptr_, gmenu_goto_ptr_;
		Gtk::MenuBar               *menubar_gtk_ptr_;
		Gtk::Menu                  *menu_goto_gtk_ptr_;
		Gtk::Toolbar               *toolbar_ptr_;

		typedef std::vector< Glib::RefPtr<Gio::SimpleAction> > actions_type;
		Glib::RefPtr<Gio::SimpleAction>
			action_open_ptr_,
			action_close_ptr_,
			action_reload_ptr_,
			action_backward_ptr_,
			action_forward_ptr_,
			action_up_ptr_,
			action_show_hidden_ptr_,
			action_complete_location_ptr_;
		actions_type actions_goto;

		void add_goto_action(
			const std::string &pathname,
			const Glib::ustring &action_name);
		void add_goto_action(
			const std::string &pathname,
			const Glib::ustring &action_name,
			const Glib::ustring &label);
	};

}  // cvn::lsgui
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6

#endif  // LS_GUI_HH
