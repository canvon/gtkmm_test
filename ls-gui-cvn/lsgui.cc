#include "lsgui.hh"
#include "lsstat.hh"
#include "lsdirent.hh"
#include "util.hh"
#include <gtkmm/dialog.h>  // For Gtk::RESPONSE_CLOSE
#include <glibmm/main.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <system_error>

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
		auto text_renderer = dynamic_cast<Gtk::CellRendererText*>(renderer);
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

	// Expand + fill horizontally, so the scroll bar appears
	// far to the right, instead of somewhere near the end
	// of the error message.
	errorMessage_.set_hexpand();
	errorMessage_.set_alignment(Gtk::ALIGN_FILL);
	scrollErrorMessage_.set_propagate_natural_width();
	scrollErrorMessage_.set_min_content_height(80);
	scrollErrorMessage_.add(errorMessage_);

	// Allow copy & paste of error messages.
	errorMessage_.set_selectable();

	auto containerptr = dynamic_cast<Gtk::Container*>(errorsInfoBar_.get_content_area());
	if (containerptr == nullptr) {
		g_warning("Can't prepare GTK InfoBar: get_content_area() is not a Gtk::Container*!");
	}
	else {
		containerptr->add(scrollErrorMessage_);
	}

	//errorsInfoBar_.add_button("_Close", 0);
	errorsInfoBar_.set_show_close_button();

	outerVBox_.pack_start(errorsInfoBar_, Gtk::PACK_SHRINK);
	// Save position for later.
	posErrorsInfoBar_ = outerVBox_.child_property_position(errorsInfoBar_).get_value();

	scrollLs_.add(ls_);
	outerVBox_.pack_start(scrollLs_);

	location_.signal_activate().connect(
		sigc::mem_fun(*this, &LsGui::on_location_activate));
	location_.signal_key_press_event().connect(
		sigc::mem_fun(*this, &LsGui::on_location_key_press_event));

	errorsInfoBar_.signal_response().connect(
		sigc::mem_fun(*this, &LsGui::on_errorsInfoBar_response));

	ls_.signal_row_activated().connect(
		sigc::mem_fun(*this, &LsGui::on_ls_row_activated));

	show_all_children();

	errorsInfoBar_.hide();

#if 0  // Don't do unrequested filesystem access, for now...
	// Switch to an initial directory...
	set_location_str("/home");
#endif
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
	add(name_raw);
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

bool LsGui::get_location_is_dirlisting() const
{
	return location_is_dirlisting_;
}

void LsGui::set_location_str(const Glib::ustring &new_location_str)
{
	location_str_ = Glib::ustring(new_location_str);
	location_is_dirlisting_ = false;

	std::cout << "New location: " << location_str_ << std::endl;

	// Put new location into location entry.
	// (Apparently this doesn't risk looping...
	// But better safe than sorry.)
	if (location_.get_text() != location_str_)
		location_.set_text(location_str_);

	// (N.B.: Be sure to use "->", as "." compiles fine
	//        but frees the smartpointer => Segmentation fault.)
	model_->clear();

	// Special-case empty string to just clear the model, without error.
	if (location_str_.empty()) {
		std::cout << "View is now empty." << std::endl;
		return;
	}

	try {
		// Retrieve stat information of the location itself.
		LsLstat loc_stat(location_str_);

		if (loc_stat.get_is_dir()) {
			location_is_dirlisting_ = true;

			std::cout << "Reading in directory "
			          << std::quoted(location_str_.raw())
			          << "..."
			          << std::endl;

			LsDirent  dir(location_str_);
			int       dir_fd = dir.fd();

			while (dir.read()) {
				std::string    ent_name = dir.get_name();
				LsFstatat      ent_stat(dir_fd, ent_name, /* symlink nofollow: */ true);

				// Put the directory entry's stat results into one row each entry.
				Gtk::TreeModel::Row row = *model_->append();
				fill_row(row, &dir_fd, ent_name, ent_stat);
			}
		}
		else {
			// Put the non-directory location's stat results into a single row.
			Gtk::TreeModel::Row row = *model_->append();
			fill_row(row, nullptr, location_str_.raw(), loc_stat);
		}
	}
	catch (std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;

		// Accumulate errors.
		errorMessages_lst_.push_back(ex.what());

		// Try to work-around GTK bug 710888,
		// InfoBar not opening again after first close.
		//
		// Remove and readd again.
		outerVBox_.remove(errorsInfoBar_);
		outerVBox_.pack_start(errorsInfoBar_, Gtk::PACK_SHRINK);
		outerVBox_.reorder_child(errorsInfoBar_, posErrorsInfoBar_);

		// Put error message into errorsInfoBar.
		Glib::ustring markup;
		int n = 1;
		const int total = errorMessages_lst_.size();
		for (errmsgs_type::const_iterator iter = errorMessages_lst_.begin();
		     iter != errorMessages_lst_.end();
		     iter++, n++)
		{
			const std::string &msg(*iter);
			gchar *msg_markup_ptr = g_markup_escape_text(msg.c_str(), -1);
			if (msg_markup_ptr == nullptr) {
				g_warning("Set location string: g_markup_escape_text() failed, will skip error message %d/%d",
					n, total);
				if (n > 1)
					markup += "\n";
				markup = markup
					+ "<big><span background=\"red\">Error "
					+ std::to_string(n) + "/" + std::to_string(total)
					+ " N.A. </span></big>";
			}
			else {
				if (n > 1)
					markup += "\n";
				markup = markup
					+ "<big>Error "
					+ std::to_string(n) + "/" + std::to_string(total)
					+ ": <span background=\"red\"> "
					+ msg_markup_ptr + " </span></big>";

				g_free(msg_markup_ptr);
				msg_markup_ptr = nullptr;
			}
		}

		errorMessage_.set_markup(markup);

		errorsInfoBar_.set_message_type(Gtk::MESSAGE_ERROR);
		errorsInfoBar_.show();

		// Scroll to bottom when things have settled.
		Glib::signal_idle().connect_once([this] {
			auto vadjustptr = scrollErrorMessage_.get_vadjustment();
			vadjustptr->set_value(vadjustptr->get_upper());
			//vadjustptr->set_value(vadjustptr->get_upper() - vadjustptr->get_page_size());
		});
	}
}

void LsGui::set_location_str_relative(const Glib::ustring &rel_path)
{
	// As a special case, on empty relative path,
	// stay with the current state.
	if (rel_path.empty())
		return;

	// Absolute path? Ignore what we had so far.
	// Also ignore if it was empty before.
	if (location_str_.empty() || rel_path[0] == '/') {
		set_location_str(rel_path);
		return;
	}

	// Otherwise, combine previous location string with relative path.
	Glib::ustring new_path(location_str_);
	if (new_path[new_path.size() - 1] != '/')
		new_path.push_back('/');
	new_path.append(rel_path);

	// Set new location string.
	set_location_str(new_path);
}

void LsGui::fill_row(Gtk::TreeModel::Row &row, const int *dirfdptr, const std::string &name, const LsStat &name_stat)
{
	row[modelColumns_.perms] = name_stat.get_mode_str();
	row[modelColumns_.nlink] = name_stat.get_nlink();
	row[modelColumns_.user]  = name_stat.get_user();
	row[modelColumns_.group] = name_stat.get_group();
	row[modelColumns_.size]  = name_stat.get_size();
	//row[modelColumns_.time]  = name_stat.get_mtime_str();  // TODO: Use when implemented.
	row[modelColumns_.name_raw] = name;

	Glib::ustring name_field(name);
	if (name_stat.get_is_lnk()) {
		try {
			if (dirfdptr)
				name_field = name_field + " -> " + cvn::readlinkat(*dirfdptr, name, name_stat);
			else
				name_field = name_field + " -> " + cvn::readlink(name, name_stat);
		}
		catch (std::system_error &ex) {
			// Special-case for system errors: Avoid information
			// that is already known via the context in which
			// the cell appears, and just display the system
			// error message.
			name_field = name_field + " (System error reading symlink target: " + ex.code().message() + ")";
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

bool LsGui::on_location_key_press_event(GdkEventKey* key_event)
{
	if (key_event) {
		switch (key_event->keyval) {
		case GDK_KEY_Escape:
			if ((key_event->state & GDK_MODIFIER_MASK) == 0) {
				// On Esc pressed on the location entry
				// with no modifiers, reset the entry text.
				location_.set_text(location_str_);
				location_.set_position(-1);
			}
			break;
		}
	}

	// Don't stop signal propagation.
	return false;
}

void LsGui::on_errorsInfoBar_response(int response_id)
{
	switch (response_id) {
	case Gtk::RESPONSE_CLOSE:
		errorMessages_lst_.clear();
		errorMessage_.set_text("");
		errorsInfoBar_.hide();
		break;
	default:
		g_warning("Errors InfoBar, signal response: Unrecognized response id %d.", response_id);
		break;
	}
}

void LsGui::on_ls_row_activated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
	auto sel_ptr = ls_.get_selection();
	if (sel_ptr->count_selected_rows() < 1)
		return;

	Gtk::TreeModel::Row row = *sel_ptr->get_selected();
	if (location_is_dirlisting_) {
		set_location_str_relative(Glib::ustring(row[modelColumns_.name_raw]));
	}
	else {
		LsStat loc_stat(location_str_);
		if (loc_stat.get_is_dir()) {
			// Append a slash '/', in the hopes to
			// dereference a directory symlink...
			set_location_str(location_str_ + '/');
		}
		else {
			// Bing!
			error_bell();

			// Stay with current state.
			return;
		}
	}
}
