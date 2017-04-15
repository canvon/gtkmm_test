#ifndef LS_GUI_HH
#define LS_GUI_HH

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/infobar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

// (Forward-declare so we don't need to include the header from here.)
class LsStat;

class LsGui : public Gtk::Window
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
		Gtk::TreeModelColumn<Glib::ustring>
			name;

		LsModelColumns();
	};

	LsModelColumns &get_modelColumns();
	Glib::RefPtr<Gtk::ListStore> get_model();
	Glib::ustring get_location_str() const;
	void set_location_str(const Glib::ustring &new_location_str);

	void fill_row(Gtk::TreeModel::Row &row, const int *dirfdptr, const Glib::ustring &name, const LsStat &name_stat);

protected:
	void on_location_activate();
	bool on_location_key_press_event(GdkEventKey* key_event);
	void on_errorsInfoBar_response(int response_id);

	LsModelColumns modelColumns_;
	Glib::RefPtr<Gtk::ListStore> model_;

	Glib::ustring location_str_;

	Gtk::Box outerVBox_;
	Gtk::Box locationHBox_;
	Gtk::Label locationLabel_;
	Gtk::Entry location_;
	Gtk::InfoBar errorsInfoBar_;
	Gtk::Label errorMessage_;
	Gtk::ScrolledWindow scrollLs_;
	Gtk::TreeView ls_;

	struct LsViewColumns
	{
		int perms, nlink, user, group, size, time, name;
	} lsViewColumns_;
};

#endif  // LS_GUI_HH
