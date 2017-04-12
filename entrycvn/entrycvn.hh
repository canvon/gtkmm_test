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
	void on_visible_toggled();
	void on_close_clicked();

	Gtk::Entry entry_;
	Gtk::Box outerBox_, middleInnerBox_;
	Gtk::CheckButton editableCheckButton_, visibleCheckButton_;
	Gtk::Button closeButton_;
};

#endif  // ENTRY_CANVON_HH
