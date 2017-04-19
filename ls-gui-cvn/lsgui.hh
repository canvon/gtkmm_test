#ifndef LS_GUI_HH
#define LS_GUI_HH

#include <vector>

#include <gtkmm/applicationwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/infobar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>
#include <gtkmm/builder.h>
#include <gtkmm/menubar.h>
#include <gtkmm/toolbar.h>

// (Forward-declare so we don't need to include the header from here.)
class LsStat;

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
	Glib::ustring get_location_str() const;
	bool get_location_is_dirlisting() const;
	void set_location_str(const Glib::ustring &new_location_str);
	void set_location_str_relative(const Glib::ustring &rel_path);

	void fill_row(Gtk::TreeModel::Row &row, const int *dirfdptr, const std::string &name, const LsStat &name_stat);

	static const char *menubar_markup;

	Glib::RefPtr<Gio::MenuModel>  get_gmenu();
	Gtk::MenuBar                 *get_menubar_gtk();
	Gtk::Toolbar                 *get_toolbar();

protected:
	void update_errorsInfoBar();

	void on_location_activate();
	bool on_location_key_press_event(GdkEventKey* key_event);
	void on_errorsInfoBar_response(int response_id);
	void on_ls_row_activated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column);
	void on_action_open();
	void on_action_close();
	void on_action_reload();
	void on_action_backward();
	void on_action_forward();

	LsModelColumns modelColumns_;
	Glib::RefPtr<Gtk::ListStore> model_;

	Glib::ustring location_str_;
	bool location_is_dirlisting_;
	typedef std::vector<std::string> errmsgs_type;
	errmsgs_type errorMessages_lst_;

	Gtk::Box outerVBox_;
	Gtk::Box locationHBox_;
	Gtk::Label locationLabel_;
	Gtk::Entry location_;
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
};

#endif  // LS_GUI_HH
