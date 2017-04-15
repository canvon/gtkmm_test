#include "lsgui.hh"
#include "lsstat.hh"
#include "lsdirent.hh"
#include "util.hh"
#include <iostream>
#include <iomanip>
#include <stdexcept>

LsGui::LsGui() :
	outerVBox_(Gtk::ORIENTATION_VERTICAL),
	locationLabel_("_Location", true)
{
	set_title("ls");
	set_default_size(800, 600);

	model_ = Gtk::ListStore::create(modelColumns_);
	ls_.set_model(model_);

	lsViewColumns_.perms = ls_.append_column("Permissions", modelColumns_.perms) - 1;
	lsViewColumns_.nlink = ls_.append_column("#links",      modelColumns_.nlink) - 1;
	lsViewColumns_.user  = ls_.append_column("User",        modelColumns_.user)  - 1;
	lsViewColumns_.group = ls_.append_column("Group",       modelColumns_.group) - 1;
	lsViewColumns_.size  = ls_.append_column("Size",        modelColumns_.size)  - 1;
	lsViewColumns_.time  = ls_.append_column("Time",        modelColumns_.time)  - 1;
	lsViewColumns_.name  = ls_.append_column("File name",   modelColumns_.name)  - 1;

	Gtk::CellRenderer *renderer = nullptr;
	if ((renderer = ls_.get_column_cell_renderer(lsViewColumns_.perms)) == nullptr) {
		g_warning("LsGui ctor: Can't get cell renderer for ls view column perms, get_column_cell_renderer() failed");
	}
	else {
		auto text_renderer = static_cast<Gtk::CellRendererText*>(renderer);
		if (text_renderer == nullptr) {
			g_warning("LsGui ctor: Can't get text cell renderer for ls view column perms, cast gave null pointer");
		}
		else {
			text_renderer->property_family().set_value("mono");
		}
	}
	if ((renderer = ls_.get_column_cell_renderer(lsViewColumns_.nlink)) == nullptr) {
		g_warning("LsGui ctor: Can't get cell renderer for ls view column nlink, get_column_cell_renderer() failed");
	}
	else {
		// right-align
		renderer->property_xalign().set_value(1.0);
	}
	if ((renderer = ls_.get_column_cell_renderer(lsViewColumns_.size)) == nullptr) {
		g_warning("LsGui ctor: Can't get cell renderer for ls view column size, get_column_cell_renderer() failed");
	}
	else {
		// right-align
		renderer->property_xalign().set_value(1.0);
	}

#if 0
	Gtk::TreeModel::Row row = *model_->append();
	row[modelColumns_.name] = "test1";
#endif

	add(outerVBox_);

	outerVBox_.pack_start(locationHBox_, Gtk::PACK_SHRINK);

	locationHBox_.pack_start(locationLabel_, Gtk::PACK_SHRINK, 10);
	locationHBox_.pack_start(location_);

	locationLabel_.set_mnemonic_widget(location_);

	scrollLs_.add(ls_);
	outerVBox_.pack_start(scrollLs_);

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

Glib::ustring LsGui::get_location_str() const
{
	return Glib::ustring(location_str_);
}

void LsGui::set_location_str(const Glib::ustring &new_location_str)
{
	location_str_ = Glib::ustring(new_location_str);

	std::cout << "New location: " << location_str_ << std::endl;

	// FIXME

	// (N.B.: Be sure to use "->", as "." compiles fine
	//        but frees the smartpointer => Segmentation fault.)
	model_->clear();

	try {
		// Retrieve stat information of the location itself.
		LsLstat loc_stat(location_str_);

		if (loc_stat.get_is_dir()) {
			std::cout << "Reading in directory "
			          << std::quoted(location_str_.raw())
			          << "..."
			          << std::endl;

			LsDirent  dir(location_str_);
			int       dir_fd = dir.fd();

			while (dir.read()) {
				Glib::ustring  ent_name = dir.get_name();
				LsFstatat      ent_stat(dir_fd, ent_name, /* symlink nofollow: */ true);

				// Put the directory entry's stat results into one row each entry.
				Gtk::TreeModel::Row row = *model_->append();
				fill_row(row, &dir_fd, ent_name, ent_stat);
			}
		}
		else {
			// Put the non-directory location's stat results into a single row.
			Gtk::TreeModel::Row row = *model_->append();
			fill_row(row, nullptr, location_str_, loc_stat);
		}
	}
	catch (std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
	}
}

void LsGui::fill_row(Gtk::TreeModel::Row &row, const int *dirfdptr, const Glib::ustring &name, const LsStat &name_stat)
{
	row[modelColumns_.perms] = name_stat.get_mode_str();
	row[modelColumns_.nlink] = name_stat.get_nlink();
	row[modelColumns_.user]  = name_stat.get_user();
	row[modelColumns_.group] = name_stat.get_group();
	row[modelColumns_.size]  = name_stat.get_size();
	//row[modelColumns_.time]  = name_stat.get_mtime_str();  // TODO: Use when implemented.

	Glib::ustring name_field(name);
	if (name_stat.get_is_lnk()) {
		try {
			if (dirfdptr)
				name_field = name_field + " -> " + cvn::readlinkat(*dirfdptr, name, name_stat);
			else
				name_field = name_field + " -> " + cvn::readlink(name, name_stat);
		}
		catch (std::exception &ex) {
			name_field = name_field + " (Error reading symlink target: " + ex.what() + ")";
		}
	}
	row[modelColumns_.name] = name_field;
}

void LsGui::on_location_activate()
{
	set_location_str(location_.get_text());
}
