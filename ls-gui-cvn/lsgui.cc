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


#define NL "\n"

const char *LsGui::menubar_markup =
   "<interface>"
NL "  <menu id='menubar'>"
NL "    <submenu>"
NL "      <attribute name='label' translatable='yes'>_File</attribute>"
NL "      <section>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>New _Tab</attribute>"
NL "          <attribute name='action'>win.newtab</attribute>"
NL "          <attribute name='accel'>&lt;Primary&gt;t</attribute>"
NL "        </item>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>_Open</attribute>"
NL "          <attribute name='action'>win.open</attribute>"
NL "          <attribute name='accel'>&lt;Primary&gt;o</attribute>"
NL "        </item>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>_Close</attribute>"
NL "          <attribute name='action'>win.close</attribute>"
NL "          <attribute name='accel'>&lt;Primary&gt;w</attribute>"
NL "        </item>"
NL "      </section>"
NL "      <section>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>_Quit</attribute>"
NL "          <attribute name='action'>app.quit</attribute>"
NL "          <attribute name='accel'>&lt;Primary&gt;q</attribute>"
NL "        </item>"
NL "      </section>"
NL "    </submenu>"
NL "    <submenu>"
NL "      <attribute name='label' translatable='yes'>_View</attribute>"
NL "      <section>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>Show _Hidden</attribute>"
NL "          <attribute name='action'>win.show-hidden</attribute>"
NL "          <attribute name='accel'>&lt;Primary&gt;h</attribute>"
NL "        </item>"
NL "      </section>"
NL "    </submenu>"
NL "    <submenu>"
NL "      <attribute name='label' translatable='yes'>_Navigation</attribute>"
NL "      <section>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>_Reload</attribute>"
NL "          <attribute name='action'>win.reload</attribute>"
NL "          <attribute name='accel'>&lt;Primary&gt;r</attribute>"
NL "        </item>"
NL "      </section>"
NL "      <section>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>_Backward</attribute>"
NL "          <attribute name='action'>win.backward</attribute>"
NL "          <attribute name='accel'>&lt;Alt&gt;Left</attribute>"
NL "        </item>"
NL "        <item>"
NL "          <attribute name='label' translatable='yes'>_Forward</attribute>"
NL "          <attribute name='action'>win.forward</attribute>"
NL "          <attribute name='accel'>&lt;Alt&gt;Right</attribute>"
NL "        </item>"
NL "      </section>"
NL "    </submenu>"
NL "  </menu>"
NL "</interface>"
NL;

LsGui::LsGui() :
	location_is_dirlisting_(false),
	location_history_pos_(location_history_.end()),
	outerVBox_(Gtk::ORIENTATION_VERTICAL),
	locationLabel_("_Location", true)
{
	set_title("ls");
	set_default_size(800, 600);

	model_ = Gtk::ListStore::create(modelColumns_);
	modelSort_ = Gtk::TreeModelSort::create(model_);
	modelSort_->set_sort_column(modelColumns_.name, Gtk::SortType::SORT_ASCENDING);
	ls_.set_model(modelSort_);

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

	ls_.set_headers_clickable();

	// React on clicking the columns, by changing the sort key.
	ls_.get_column(lsViewColumns_.perms)->signal_clicked().connect([this] {
			modelSort_->set_sort_column(modelColumns_.perms, Gtk::SortType::SORT_ASCENDING);
	});
	ls_.get_column(lsViewColumns_.nlink)->signal_clicked().connect([this] {
			modelSort_->set_sort_column(modelColumns_.nlink, Gtk::SortType::SORT_ASCENDING);
	});
	ls_.get_column(lsViewColumns_.user)->signal_clicked().connect([this] {
			modelSort_->set_sort_column(modelColumns_.user, Gtk::SortType::SORT_ASCENDING);
	});
	ls_.get_column(lsViewColumns_.group)->signal_clicked().connect([this] {
			modelSort_->set_sort_column(modelColumns_.group, Gtk::SortType::SORT_ASCENDING);
	});
	ls_.get_column(lsViewColumns_.size)->signal_clicked().connect([this] {
			modelSort_->set_sort_column(modelColumns_.size, Gtk::SortType::SORT_ASCENDING);
	});
	ls_.get_column(lsViewColumns_.time)->signal_clicked().connect([this] {
			modelSort_->set_sort_column(modelColumns_.time, Gtk::SortType::SORT_ASCENDING);
	});
	ls_.get_column(lsViewColumns_.name)->signal_clicked().connect([this] {
			modelSort_->set_sort_column(modelColumns_.name, Gtk::SortType::SORT_ASCENDING);
	});

#if 0
	Gtk::TreeModel::Row row = *model_->append();
	row[modelColumns_.name] = "test1";
#endif


	add(outerVBox_);


	// Load menubar and/or toolbar.

	builder_ptr_ = Gtk::Builder::create();

	try {
		builder_ptr_->add_from_string(menubar_markup);
		builder_ptr_->add_from_resource("/toolbar/toolbar.glade");
	}
	catch (const Glib::Error &ex)
	{
		auto errmsg = Glib::ustring("Building menu bar & tool bar failed: ") + ex.what();
		g_warning("%s", errmsg.c_str());
		errorMessages_lst_.push_back(errmsg);
	}

	// menubar
	do {
		auto obj_ptr = builder_ptr_->get_object("menubar");
		if (!obj_ptr) {
			Glib::ustring errmsg("Object 'menubar' not found");
			g_warning("%s", errmsg.c_str());
			errorMessages_lst_.push_back(errmsg);
			break;
		}

		gmenu_ptr_ = Glib::RefPtr<Gio::Menu>::cast_dynamic(obj_ptr);
		if (!gmenu_ptr_) {
			Glib::ustring errmsg("Object 'menubar' is not a Gio::Menu");
			g_warning("%s", errmsg.c_str());
			errorMessages_lst_.push_back(errmsg);
			break;
		}

#if 0
		// As the menubar is now application-wide and we're deriving
		// from Gtk::ApplicationWindow, creating a Gtk::MenuBar is
		// not necessary any longer and will just lead to a duplicate
		// menubar...
		menubar_gtk_ptr_ = Gtk::manage(new Gtk::MenuBar(gmenu_ptr_));
		outerVBox_.pack_start(*menubar_gtk_ptr_, Gtk::PACK_SHRINK);
#else
		menubar_gtk_ptr_ = nullptr;
#endif

#if 0
		// This will not work as we are still in the constructor,
		// and if we didn't get the application instance passed
		// as constructor argument, we don't have an application, yet.
		auto app = get_application();
		if (!app) {
			Glib::ustring errmsg("Couldn't get application instance");
			g_warning("%s", errmsg.c_str());
			errorMessages_lst_.push_back(errmsg);
			break;
		}

		app->set_menubar(gmenu_ptr);
#endif
	}
	while (false);

	toolbar_ptr_ = nullptr;
	builder_ptr_->get_widget("toolbar", toolbar_ptr_);  // implicitly managed
	if (!toolbar_ptr_) {
		Glib::ustring errmsg("Couldn't get widget 'toolbar'");
		g_warning("%s", errmsg.c_str());
		errorMessages_lst_.push_back(errmsg);
	}
	else {
		outerVBox_.pack_start(*toolbar_ptr_, Gtk::PACK_SHRINK);
	}


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

	// These will be prefixed "win.":
	action_open_ptr_ = add_action("open", sigc::mem_fun(*this, &LsGui::on_action_open));
	action_close_ptr_ = add_action("close", sigc::mem_fun(*this, &LsGui::on_action_close));
	action_reload_ptr_ = add_action("reload", sigc::mem_fun(*this, &LsGui::on_action_reload));
	action_backward_ptr_ = add_action("backward", sigc::mem_fun(*this, &LsGui::on_action_backward));
	action_forward_ptr_ = add_action("forward", sigc::mem_fun(*this, &LsGui::on_action_forward));
	action_show_hidden_ptr_ = add_action_bool("show-hidden", sigc::mem_fun(*this, &LsGui::on_action_show_hidden));

	update_actions();


	show_all_children();

	update_errorsInfoBar();

	location_.grab_focus();

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

bool LsGui::history_is_valid() const
{
	if (location_history_.empty())
		return false;
	if (location_history_pos_ == location_history_.end())
		return false;
	// TODO: Possibly add further checks?

	return true;
}

bool LsGui::history_can_backward() const
{
	if (location_history_.empty())
		return false;
	if (location_history_pos_ == location_history_.begin())
		return false;

	return true;
}

bool LsGui::history_can_forward() const
{
	if (location_history_.empty())
		return false;
	if (location_history_pos_ == location_history_.end())
		return false;

	location_history_type::const_iterator last_elem_ptr = location_history_.end();
	last_elem_ptr--;
	if (location_history_pos_ == last_elem_ptr)
		return false;

	return true;
}

void LsGui::update_actions()
{
	action_reload_ptr_->set_enabled(history_is_valid());
	action_backward_ptr_->set_enabled(history_can_backward());
	action_forward_ptr_->set_enabled(history_can_forward());
}

Glib::ustring LsGui::get_location_str() const
{
	return Glib::ustring(location_str_);
}

bool LsGui::get_location_is_dirlisting() const
{
	return location_is_dirlisting_;
}

void LsGui::set_location_str()
{
	if (!history_is_valid()) {
		std::cerr << "We're outside history!" << std::endl;

#if 0
		// Go on with the empty location.
		location_str_ = "";
#else
		// Indicate input error (Is that really right?),
		// and stay at current state.
		error_bell();
		return;
#endif
	}
	else {
		// Fetch location from current history position.
		location_str_ = Glib::ustring(*location_history_pos_);
	}

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
			bool show_hidden = get_show_hidden();

			std::cout << "Reading in directory "
			          << std::quoted(location_str_.raw())
			          << "..."
			          << std::endl;

			LsDirent  dir(location_str_);
			int       dir_fd = dir.fd();

			while (dir.read()) {
				std::string    ent_name = dir.get_name();
				LsFstatat      ent_stat(dir_fd, ent_name, /* symlink nofollow: */ true);

				if (!show_hidden && !ent_name.empty() && ent_name[0] == '.') {
					// Skip hidden files/directories.
					continue;
				}

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

		// Display error to the user in a nice InfoBar
		// that can be closed when the user pleases
		// (compared to a message dialog, which must be
		// closed immediately to get any more work done).
		update_errorsInfoBar();
	}
}

void LsGui::set_location_str(const Glib::ustring &new_location_str)
{
	// Nothing in the history, yet?
	if (location_history_pos_ == location_history_.end()) {
		// Simply insert as last element, and go
		// from one-past-the-last-element to the last element.
		location_history_.push_back(new_location_str);
		location_history_pos_--;
	}
	else {
		// Erase forward history, in case there is one.
		location_history_type::iterator to_erase = location_history_pos_;
		to_erase++;
		location_history_.erase(to_erase, location_history_.end());

		// Add new location as new history element,
		// and advance position to point to it.
		location_history_.push_back(new_location_str);
		location_history_pos_++;
	}

	update_actions();

	set_location_str();
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

Glib::RefPtr<Gio::MenuModel> LsGui::get_gmenu()
{
	return gmenu_ptr_;
}

Gtk::MenuBar *LsGui::get_menubar_gtk()
{
	return menubar_gtk_ptr_;
}

Gtk::Toolbar *LsGui::get_toolbar()
{
	return toolbar_ptr_;
}

bool LsGui::get_show_hidden() const
{
	bool show_hidden = false;
	action_show_hidden_ptr_->get_state(show_hidden);
	return show_hidden;
}

void LsGui::update_errorsInfoBar()
{
	if (errorMessages_lst_.empty())
		errorsInfoBar_.hide();
	else {
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
		}, Glib::PRIORITY_LOW);
	}
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
	if (sel_ptr->count_selected_rows() < 1) {
		ls_.error_bell();
		return;
	}

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
			// "Bing!" & stay with current state.
			ls_.error_bell();
		}
	}
}

void LsGui::on_action_open()
{
	// FIXME
	g_warning("Directory -> Open: Not implemented, yet!");
}

void LsGui::on_action_close()
{
	// FIXME
	g_warning("Directory -> Close: Not implemented, yet!");
}

void LsGui::on_action_reload()
{
	set_location_str();
}

void LsGui::on_action_backward()
{
	if (!history_can_backward()) {
		std::cerr << "Can't go backwards in history!" << std::endl;
		error_bell();
		return;
	}

	std::cout << "Going backwards in history..." << std::endl;
	location_history_pos_--;
	update_actions();
	set_location_str();
}

void LsGui::on_action_forward()
{
	if (!history_can_forward()) {
		std::cerr << "Can't go forward in history!" << std::endl;
		error_bell();
		return;
	}

	std::cout << "Going forward in history..." << std::endl;
	location_history_pos_++;
	update_actions();
	set_location_str();
}

void LsGui::on_action_show_hidden()
{
	// Toggle state of the action.
	bool show_hidden_new = !get_show_hidden();
	std::cout << "Toggling show hidden to " << show_hidden_new << "..." << std::endl;
	action_show_hidden_ptr_->change_state(show_hidden_new);

	if (location_is_dirlisting_) {
		// Re-read directory with new state.
		set_location_str();
	}
}
