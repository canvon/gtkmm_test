#ifndef LS_GUI_HH
#define LS_GUI_HH

#include <list>
#include <vector>

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
#include <giomm/simpleaction.h>

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
			Gtk::TreeModelColumn<Glib::ustring>
				time;  // mtime, ctime or atime
			Gtk::TreeModelColumn<std::string>
				name_raw;
			Gtk::TreeModelColumn<Glib::ustring>
				name;

			LsModelColumns();
		};

		LsModelColumns &get_modelColumns();
		Glib::RefPtr<Gtk::ListStore> get_model();

		void display_errmsg(const Glib::ustring &errmsg);
		void display_glib_msg(
			const Glib::ustring &log_domain,
			GLogLevelFlags log_level,
			const Glib::ustring &msg,
			const GLogField *fields);

		bool history_is_valid() const;
		bool history_can_backward() const;
		bool history_can_forward() const;

		void history_clear();
		void history_add(const Glib::ustring &new_item_str);

		Glib::ustring get_location_str() const;
		bool get_location_is_dirlisting() const;
		void set_location_str();
		void set_location_str(const Glib::ustring &new_location_str);
		void set_location_str_relative(const Glib::ustring &rel_path);

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

		void on_locationEntry_activate();
		void on_locationEntry_changed();
		bool on_locationEntry_key_press_event(GdkEventKey* key_event);
		void on_locationEntryCompletion_no_matches();
		void on_errorsInfoBar_response(int response_id);
		void on_ls_row_activated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
		void on_ls_column_clicked(int columnNr);
		void on_action_open();
		void on_action_close();
		void on_action_reload();
		void on_action_backward();
		void on_action_forward();
		void on_action_show_hidden();
		void on_action_complete_location();

		LsModelColumns modelColumns_;
		Glib::RefPtr<Gtk::ListStore> model_;
		Glib::RefPtr<Gtk::TreeModelSort> modelSort_;

		Glib::ustring location_str_;
		bool location_is_dirlisting_;
		bool location_esc_pressed_;

		typedef std::list<Glib::ustring> location_history_type;
		location_history_type            location_history_;
		location_history_type::iterator  location_history_pos_;

		typedef std::vector<std::string> errmsgs_type;
		errmsgs_type errorMessages_lst_;

		Gtk::Box outerVBox_;
		Gtk::Box locationHBox_;
		Gtk::Label locationLabel_;
		Gtk::Entry location_;
		Glib::RefPtr<Gtk::ListStore> locationCompletionModel_ptr_;
		Glib::RefPtr<Gtk::EntryCompletion> locationCompletion_ptr_;
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
		Glib::RefPtr<Gio::Menu>     gmenu_ptr_;
		Gtk::MenuBar               *menubar_gtk_ptr_;
		Gtk::Toolbar               *toolbar_ptr_;

		Glib::RefPtr<Gio::SimpleAction>
			action_open_ptr_,
			action_close_ptr_,
			action_reload_ptr_,
			action_backward_ptr_,
			action_forward_ptr_,
			action_show_hidden_ptr_,
			action_complete_location_ptr_;
	};

}  // cvn::lsgui
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6

#endif  // LS_GUI_HH
