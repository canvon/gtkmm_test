#include "lsgui.hh"
#include "lsstat.hh"
#include "lsdirent.hh"
#include <iostream>
#include <iomanip>
#include <stdexcept>

LsGui::LsGui() :
	outerVBox_(Gtk::ORIENTATION_VERTICAL)
{
	set_title("ls");

	model_ = Gtk::ListStore::create(modelColumns_);
	ls_.set_model(model_);

	ls_.append_column("Permissions", modelColumns_.perms);
	ls_.append_column("#links",      modelColumns_.nlink);
	ls_.append_column("User",        modelColumns_.user);
	ls_.append_column("Group",       modelColumns_.group);
	ls_.append_column("Size",        modelColumns_.size);
	ls_.append_column("Time",        modelColumns_.time);
	ls_.append_column("File name",   modelColumns_.name);

	Gtk::TreeModel::Row row = *model_->append();
	row[modelColumns_.name] = "test1";

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
	add(user); add(group);
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

	// (N.B.: Be sure to use "->", as "." compiles fine
	//        but frees the smartpointer => Segmentation fault.)
	model_->clear();

	try {
		// Retrieve stat information of the location itself.
		LsLstat loc_stat(loc);

		if (loc_stat.get_is_dir()) {
			std::cout << "Reading in directory "
			          << std::quoted(loc.raw())
			          << "..."
			          << std::endl;

			LsDirent dir(loc);

			while (dir.read()) {
				Glib::ustring ent_name = dir.get_name();
				LsLstat ent_stat(ent_name);

				// Put the directory entry's stat results into the row.
				Gtk::TreeModel::Row row = *model_->append();
				row[modelColumns_.perms] = ent_stat.get_mode_str();
				row[modelColumns_.nlink] = ent_stat.get_nlink();
				row[modelColumns_.user]  = ent_stat.get_user();
				row[modelColumns_.group] = ent_stat.get_group();
				row[modelColumns_.size]  = ent_stat.get_size();
				//row[modelColumns_.time]  = ent_stat.get_mtime_str();  // TODO: Use when implemented.
				row[modelColumns_.name]  = ent_name;
			}
		}
		else {
			// Put the non-directory location's stat results into the row.
			Gtk::TreeModel::Row row = *model_->append();
			row[modelColumns_.perms] = loc_stat.get_mode_str();
			row[modelColumns_.nlink] = loc_stat.get_nlink();
			row[modelColumns_.user]  = loc_stat.get_user();
			row[modelColumns_.group] = loc_stat.get_group();
			row[modelColumns_.size]  = loc_stat.get_size();
			//row[modelColumns_.time]  = loc_stat.get_mtime_str();  // TODO: Use when implemented.
			row[modelColumns_.name]  = loc;
		}
	}
	catch (std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
	}
}
