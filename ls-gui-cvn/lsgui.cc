#include "lsgui.hh"
#include <iostream>

LsGui::LsGui() :
	outerVBox_(Gtk::ORIENTATION_VERTICAL)
{
	set_title("ls");

	add(outerVBox_);

	outerVBox_.pack_start(location_);
	outerVBox_.pack_start(ls_);

	location_.signal_activate().connect(
		sigc::mem_fun(*this, &LsGui::on_location_activate));

	// FIXME

	show_all_children();
}

LsGui::~LsGui()
{
}

void LsGui::on_location_activate()
{
	Glib::ustring loc = location_.get_text();

	std::cout << "New location: " << loc << std::endl;

	// FIXME
}
