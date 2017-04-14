#include "lsgui.hh"
#include <iostream>

LsGui::LsGui() :
	outerVBox_(Gtk::ORIENTATION_VERTICAL)
{
	set_title("ls");

	model_ = Gtk::ListStore::create(modelColumns_);
	ls_.set_model(model_);

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

LsGui::LsModelColumns::LsModelColumns()
{
	add(perms);
	add(nlink);
	add(owner); add(group);
	add(size);
	add(time);
	add(name);
}

LsGui::LsModelColumns &LsGui::get_modelColumns()
{
	return modelColumns_;
}

Glib::RefPtr<Gtk::ListStore> LsGui::get_model()
{
	return model_;
}

void LsGui::on_location_activate()
{
	Glib::ustring loc = location_.get_text();

	std::cout << "New location: " << loc << std::endl;

	// FIXME
}
