#ifndef LS_GUI_HH
#define LS_GUI_HH

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>

class LsGui : public Gtk::Window
{
public:
	LsGui();
	virtual ~LsGui();

protected:
	void on_location_activate();

	Gtk::Box outerVBox_;
	Gtk::Entry location_;
	Gtk::TreeView ls_;
};

#endif  // LS_GUI_HH
