#define G_LOG_USE_STRUCTURED

#include "lsgui.hh"
#include "stat-cvn.hh"
#include "dirent-cvn.hh"
#include "util.hh"
#include <gtkmm/dialog.h>  // For Gtk::RESPONSE_CLOSE
#include <gtkmm/filechooserdialog.h>
#include <glibmm/convert.h>
#include <glibmm/main.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <system_error>

#include "versioncheck.hh"


#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "lsgui"

//namespace cvn::lsgui
namespace cvn { namespace lsgui
{

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
	NL "    <submenu>"
	NL "      <attribute name='label' translatable='yes'>_Options</attribute>"
	NL "      <section>"
	NL "        <item>"
	NL "          <attribute name='label' translatable='yes'>_Complete Location</attribute>"
	NL "          <attribute name='action'>win.complete-location</attribute>"
	NL "        </item>"
	NL "      </section>"
	NL "    </submenu>"
	NL "  </menu>"
	NL "</interface>"
	NL;

	LsGui::LsGui() :
		location_is_dirlisting_(false),
		location_esc_pressed_(false),
		location_history_pos_(location_history_.end()),
		outerVBox_(Gtk::ORIENTATION_VERTICAL),
		locationLabel_("_Location", true)
	{
		auto warn = [this](const Glib::ustring &msg) {
			g_warning("%s", msg.c_str());
			// As we're still in the constructor,
			// the glib structed log writer installed in the main module
			// will not have access to us via the Gtk::Application instance.
			// So we'll have to update our InfoBar ourselves.
			errorMessages_lst_.push_back(ErrMsg { G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, msg });
		};

		set_title_addition();
		set_default_size(800, 600);

		model_ = Gtk::ListStore::create(modelColumns_);
		modelSort_ = Gtk::TreeModelSort::create(model_);
		modelSort_->set_sort_column(modelColumns_.name, Gtk::SortType::SORT_ASCENDING);
		ls_.set_model(modelSort_);

		locationCompletionModel_ptr_ = Gtk::ListStore::create(modelColumns_);
		locationCompletionModel_ptr_->set_sort_column(
			modelColumns_.name_raw, Gtk::SortType::SORT_ASCENDING);
		locationCompletion_ptr_ = Gtk::EntryCompletion::create();
		locationCompletion_ptr_->set_model(locationCompletionModel_ptr_);
		locationCompletion_ptr_->set_text_column(modelColumns_.name_raw);
		location_.set_completion(locationCompletion_ptr_);

		lsViewColumns_.perms = ls_.append_column("Permissions", modelColumns_.perms) - 1;
		lsViewColumns_.nlink = ls_.append_column("#links",      modelColumns_.nlink) - 1;
		lsViewColumns_.user  = ls_.append_column("User",        modelColumns_.user)  - 1;
		lsViewColumns_.group = ls_.append_column("Group",       modelColumns_.group) - 1;
		lsViewColumns_.size  = ls_.append_column("Size",        modelColumns_.size)  - 1;
		lsViewColumns_.time  = ls_.append_column("Time",        modelColumns_.time)  - 1;
		lsViewColumns_.name  = ls_.append_column("File name",   modelColumns_.name)  - 1;

		Gtk::CellRenderer *renderer = nullptr;
		if ((renderer = ls_.get_column_cell_renderer(lsViewColumns_.perms)) == nullptr) {
			warn("LsGui ctor: Can't get cell renderer for ls view column perms, get_column_cell_renderer() failed");
		}
		else {
			auto text_renderer = dynamic_cast<Gtk::CellRendererText*>(renderer);
			if (text_renderer == nullptr) {
				warn("LsGui ctor: Can't get text cell renderer for ls view column perms, cast gave null pointer");
			}
			else {
				text_renderer->property_family().set_value("mono");
			}
		}
		if ((renderer = ls_.get_column_cell_renderer(lsViewColumns_.nlink)) == nullptr) {
			warn("LsGui ctor: Can't get cell renderer for ls view column nlink, get_column_cell_renderer() failed");
		}
		else {
			// right-align
			renderer->property_xalign().set_value(1.0);
		}
		if ((renderer = ls_.get_column_cell_renderer(lsViewColumns_.size)) == nullptr) {
			warn("LsGui ctor: Can't get cell renderer for ls view column size, get_column_cell_renderer() failed");
		}
		else {
			// right-align
			renderer->property_xalign().set_value(1.0);
		}

		ls_.set_headers_clickable();

		// React on clicking the columns, by changing the sort key.
		for (int columnNr = lsViewColumns_.perms; columnNr <= lsViewColumns_.name; columnNr++)
		{
			ls_.get_column(columnNr)->signal_clicked().connect(
				sigc::bind(
					sigc::mem_fun(*this, &LsGui::on_ls_column_clicked),
					columnNr));
		}

#if 0
		Gtk::TreeModel::Row row = *model_->append();
		row[modelColumns_.name] = "test1";
#endif


		add(outerVBox_);


		// Load menubar and/or toolbar.

		builder_ptr_ = Gtk::Builder::create();

		// menubar
		do {
			try {
				builder_ptr_->add_from_string(menubar_markup);
			}
			catch (const Glib::Error &ex)
			{
				warn(Glib::ustring("Building menu bar failed: ") + ex.what());
				break;
			}

			auto obj_ptr = builder_ptr_->get_object("menubar");
			if (!obj_ptr) {
				warn("Object 'menubar' not found");
				break;
			}

			gmenu_ptr_ = Glib::RefPtr<Gio::Menu>::cast_dynamic(obj_ptr);
			if (!gmenu_ptr_) {
				warn("Object 'menubar' is not a Gio::Menu");
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
				warn("Couldn't get application instance");
				break;
			}

			app->set_menubar(gmenu_ptr);
#endif
		}
		while (false);

		// toolbar
		do {
			try {
				builder_ptr_->add_from_resource("/toolbar/toolbar.glade");
			}
			catch (const Glib::Error &ex)
			{
				warn(Glib::ustring("Building tool bar failed: ") + ex.what());
				break;
			}

			toolbar_ptr_ = nullptr;
			builder_ptr_->get_widget("toolbar", toolbar_ptr_);  // implicitly managed
			if (!toolbar_ptr_) {
				warn("Couldn't get widget 'toolbar'");
				break;
			}

			outerVBox_.pack_start(*toolbar_ptr_, Gtk::PACK_SHRINK);
		}
		while (false);


		outerVBox_.pack_start(locationHBox_, Gtk::PACK_SHRINK);

		locationHBox_.pack_start(locationLabel_, Gtk::PACK_SHRINK, 10);
		locationHBox_.pack_start(location_);

		locationLabel_.set_mnemonic_widget(location_);

		// Expand + fill horizontally, so the scroll bar appears
		// far to the right, instead of somewhere near the end
		// of the error message.
		errorMessage_.set_hexpand();
		errorMessage_.set_alignment(Gtk::ALIGN_FILL);
#if GTKMM_VERSION_GE(3,22)
		scrollErrorMessage_.set_propagate_natural_width();
#else
#pragma message("Warning: Skipping call to Gtk::ScrolledWindow::set_propagate_natural_width()," \
	" requires gtkmm 3.22.0 but compiling against " GTKMM_VERSION_STRING)

		warn("Skipping call to Gtk::ScrolledWindow::set_propagate_natural_width(),"
			" requires gtkmm 3.22.0 but was compiled against %s",
			GTKMM_VERSION_STRING);
#endif
		scrollErrorMessage_.set_min_content_height(80);
		scrollErrorMessage_.add(errorMessage_);

		// Allow copy & paste of error messages.
		errorMessage_.set_selectable();
		// Allow error messages to wrap, so the user does not need to
		// scroll to read the important part of the message.
		errorMessage_.set_line_wrap();

		auto containerptr = dynamic_cast<Gtk::Container*>(errorsInfoBar_.get_content_area());
		if (containerptr == nullptr) {
			warn("Can't prepare GTK InfoBar: get_content_area() is not a Gtk::Container*!");
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
			sigc::mem_fun(*this, &LsGui::on_locationEntry_activate));
		location_.signal_changed().connect(
			sigc::mem_fun(*this, &LsGui::on_locationEntry_changed));
		location_.signal_key_press_event().connect(
			sigc::mem_fun(*this, &LsGui::on_locationEntry_key_press_event));
		locationCompletion_ptr_->signal_no_matches().connect(
			sigc::mem_fun(*this, &LsGui::on_locationEntryCompletion_no_matches));

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
		//
		// Stateful actions.
		action_show_hidden_ptr_ = add_action_bool("show-hidden",
			sigc::mem_fun(*this, &LsGui::on_action_show_hidden));
		action_complete_location_ptr_ = add_action_bool("complete-location",
			sigc::mem_fun(*this, &LsGui::on_action_complete_location),
			true);

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

	void LsGui::display_errmsg(const Glib::ustring &errmsg)
	{
		display_glib_msg(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, errmsg, nullptr);
	}

	void LsGui::display_glib_msg(
		const Glib::ustring &log_domain,
		GLogLevelFlags log_level,
		const Glib::ustring &msg,
		const GLogField *fields)
	{
		// Accumulate messages.
#if 0
		errorMessages_lst_.push_back(
			((log_domain.empty() || log_domain == G_LOG_DOMAIN)
				? "" : "From " + log_domain + ": ")
			+ msg);
#else
		errorMessages_lst_.push_back(ErrMsg { log_domain, log_level, msg });
#endif

		// Display message to the user in a nice InfoBar
		// that can be closed when the user pleases
		// (compared to a message dialog, which must be
		// closed immediately to get any more work done).
		update_errorsInfoBar();
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

		// (Don't advertise an ability to further close something
		// when there is nothing left to close, as the history is empty.)
		action_close_ptr_->set_enabled(!location_history_.empty());
	}

	void LsGui::history_clear()
	{
		// Restore state after ctor, hopefully.
		location_history_.clear();
		location_history_pos_ = location_history_.end();

		update_actions();
	}

	void LsGui::history_add(const Glib::ustring &new_item_str)
	{
		// Nothing in the history, yet?
		if (location_history_pos_ == location_history_.end()) {
			// Simply insert as last element, and go
			// from one-past-the-last-element to the last element.
			location_history_.push_back(new_item_str);
			location_history_pos_--;
		}
		else {
			// Erase forward history, in case there is one.
			location_history_type::iterator to_erase = location_history_pos_;
			to_erase++;
			location_history_.erase(to_erase, location_history_.end());

			// Add new location as new history element,
			// and advance position to point to it.
			location_history_.push_back(new_item_str);
			location_history_pos_++;
		}

		update_actions();
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
		if (location_history_.empty()) {
			// Go on with the empty location.
			location_str_ = "";
		}
		else if (!history_is_valid()) {
			std::cerr << "We're outside history!" << std::endl;

			// Indicate input error (Is that really right?),
			// and stay at current state.
			error_bell();
			return;
		}
		else {
			// Fetch location from current history position.
			location_str_ = Glib::ustring(*location_history_pos_);
		}

		location_is_dirlisting_ = false;

		std::cout << "New location: " << std::quoted(location_str_.raw()) << std::endl;

		// Put new location into location entry.
		// (Apparently this doesn't risk looping...
		// But better safe than sorry.)
		if (location_.get_text() != location_str_)
			location_.set_text(location_str_);

		// Update window title as well.
		// FIXME: Give an opsys-encoded std::string to filename_display_basename()!
		set_title_addition(
			location_str_,
			location_str_.empty() ? "" : Glib::filename_display_basename(location_str_));

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
			cvn::fs::Lstat  loc_stat(location_str_);

			if (loc_stat.get_is_dir()) {
				location_is_dirlisting_ = true;
				bool show_hidden = get_show_hidden();

				std::cout << "Reading in directory "
				          << std::quoted(location_str_.raw())
				          << "..."
				          << std::endl;

				cvn::fs::Dirent  dir(location_str_);
				int              dir_fd = dir.fd();

				while (dir.read()) {
					std::string       ent_name = dir.get_ent_name();
					cvn::fs::Fstatat  ent_stat(dir_fd, ent_name, /* symlink nofollow: */ true);

					if (!show_hidden && cvn::fs::is_hidden(dir_fd, ent_name)) {
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

			display_errmsg(ex.what());
		}
	}

	void LsGui::set_location_str(const Glib::ustring &new_location_str)
	{
		// (Prevent entering the same location into history
		// multiple times.)
		if (location_history_pos_ == location_history_.end() ||
		    new_location_str != *location_history_pos_)
			history_add(new_location_str);

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

	void LsGui::fill_row(Gtk::TreeModel::Row &row,
		const int *dirfdptr,
		const std::string &name,
		const cvn::fs::Stat &name_stat)
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
					name_field = name_field + " -> " + cvn::fs::readlinkat(*dirfdptr, name, name_stat);
				else
					name_field = name_field + " -> " + cvn::fs::readlink(name, name_stat);
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

	bool LsGui::get_complete_location() const
	{
		bool complete_location = true;
		action_complete_location_ptr_->get_state(complete_location);
		return complete_location;
	}

	void LsGui::set_title_addition(
		const Glib::ustring &title_addition,
		const Glib::ustring &short_name)
	{
		Glib::ustring title("ls");
		if (!title_addition.empty())
			title = title_addition + " - " + title;
		if (!short_name.empty())
			title = "[" + short_name + "] " + title;
		set_title(title);
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
			// (But protect against doing this when already visible,
			// as its visualization becomes empty when overdoing this.)
			if (!errorsInfoBar_.is_visible()) {
				outerVBox_.remove(errorsInfoBar_);
				outerVBox_.pack_start(errorsInfoBar_, Gtk::PACK_SHRINK);
				outerVBox_.reorder_child(errorsInfoBar_, posErrorsInfoBar_);
			}

			// Put error message into errorsInfoBar.
			Glib::ustring markup;
			int n = 1;
			const int total = errorMessages_lst_.size();
			for (errmsgs_type::const_iterator iter = errorMessages_lst_.begin();
			     iter != errorMessages_lst_.end();
			     iter++, n++)
			{
				GLogLevelFlags level = iter->log_level;
				const Glib::ustring &domain(iter->log_domain), &msg(iter->msg);
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
					gchar *domain_markup_ptr = nullptr;
					if (!domain.empty() && domain != G_LOG_DOMAIN)
						domain_markup_ptr = g_markup_escape_text(domain.c_str(), -1);

					Glib::ustring msg_type_name("(Unknown message type)");
					switch (level & G_LOG_LEVEL_MASK) {
					case G_LOG_LEVEL_DEBUG:     msg_type_name = "Debug";       break;
					case G_LOG_LEVEL_INFO:      msg_type_name = "Info";        break;
					case G_LOG_LEVEL_MESSAGE:   msg_type_name = "Message";     break;
					case G_LOG_LEVEL_WARNING:   msg_type_name = "Warning";     break;
					case G_LOG_LEVEL_CRITICAL:  msg_type_name = "Critical";    break;
					case G_LOG_LEVEL_ERROR:     msg_type_name = "Fatal error"; break;
					}

					if (n > 1)
						markup += "\n";
					markup = markup
						+ "<big>"
						+ std::to_string(n) + "/" + std::to_string(total) + ": "
						+ (domain_markup_ptr ? Glib::ustring("From ") + domain_markup_ptr + ": " : "")
						+ msg_type_name + ": <span"
						+ (((level & G_LOG_LEVEL_MASK) <= G_LOG_LEVEL_WARNING)
							? " background=\"red\"" : "")
						+ "> "
						+ msg_markup_ptr + " </span></big>";

					if (domain_markup_ptr) {
						g_free(domain_markup_ptr);
						domain_markup_ptr = nullptr;
					}
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

	void LsGui::update_locationCompletion()
	{
		locationCompletionModel_ptr_->clear();

		Glib::ustring typed_str = location_.get_text();
		Glib::ustring::size_type pos_slash = typed_str.rfind("/");

		Glib::ustring dir_path("."), rel_name(typed_str);
		bool prepend_dir_path = false;
		if (pos_slash != Glib::ustring::npos &&
			pos_slash >= 0 && typed_str[pos_slash] == '/')
		{
			dir_path = typed_str.substr(0, pos_slash + 1);
			prepend_dir_path = true;

			if (pos_slash + 1 < typed_str.length())
				rel_name = typed_str.substr(pos_slash + 1);
			else
				rel_name.clear();
		}

		bool show_hidden = get_show_hidden();

		try {
			cvn::fs::Dirent  dir(dir_path);
			int              dir_fd = dir.fd();

			while (dir.read()) {
				std::string ent_name = dir.get_ent_name();
				if (ent_name == "." ||
				    ent_name == "..") {
					// Skip current directory and parent directory
					// for completion as they usually will not help.
					continue;
				}

				if (!show_hidden && cvn::fs::is_hidden(dir_fd, ent_name)) {
					if (rel_name.empty() || rel_name != ent_name.substr(0, rel_name.length())) {
						// Skip hidden files/directories
						// that have not been entered explicitly.
						continue;
					}
				}

				Gtk::TreeModel::Row row = *locationCompletionModel_ptr_->append();
				if (prepend_dir_path)
					row[modelColumns_.name_raw] = dir_path + ent_name;
				else
					row[modelColumns_.name_raw] = ent_name;
			}
		}
		catch (const std::system_error &ex) {
			if (ex.code().category() == std::generic_category()) {
				switch (static_cast<std::errc>(ex.code().value())) {
				case std::errc::no_such_file_or_directory:
					// Ignore.
					break;
				default:
#if 0
					// (Don't use what() so we skip unnecessary details.
					// This is just a completion error, after all...)
					// TODO: (Perhaps this shouldn't log anything at all?)
					std::cerr << "Location completion: Got system error: "
					          << ex.code().message() << std::endl;
#else
					// (Use full exception message as it's
					// really not helpful to see this
					// on stderr without.)
					std::cerr << "Location completion: Got system error: "
					          << ex.what() << std::endl;
#endif
					break;
				}
			}
			else {
				std::cerr << "Location completion: Got system error of non-generic category: "
				          << ex.what() << std::endl;
			}
		}
		catch (const std::exception &ex) {
			std::cerr << "Location completion: Got exception: "
			          << ex.what() << std::endl;
		}
	}

	void LsGui::on_locationEntry_activate()
	{
		set_location_str(location_.get_text());
	}

	void LsGui::on_locationEntry_changed()
	{
		Glib::ustring text = location_.get_text();
		if (text.empty())
			return;

		int pos = location_.get_position();
		if (text[pos] == '/') {
			// It's possible the user has just input a slash
			// (as in "directory separator"), so try to rebuild
			// the completion cache.
			//
			// TODO: Let this happen in an idle handler or background thread.
			update_locationCompletion();
		}
	}

	bool LsGui::on_locationEntry_key_press_event(GdkEventKey* key_event)
	{
		if (key_event) {
			switch (key_event->keyval) {
			case GDK_KEY_Escape:
				if ((key_event->state & GDK_MODIFIER_MASK) == 0) {
					if (location_esc_pressed_) {
						// On Esc pressed on the location entry
						// with no modifiers, reset the entry text.
						location_.set_text(location_str_);
						location_.set_position(-1);
						location_esc_pressed_ = false;
					}
					else {
						// ..but only if it has been hit twice.
						// This lets a single press close the
						// location entry completion window.
						location_esc_pressed_ = true;
					}
				}
				else
					location_esc_pressed_ = false;
				break;
			default:
				location_esc_pressed_ = false;
				break;
			}
		}

		// Don't stop signal propagation.
		return false;
	}

	void LsGui::on_locationEntryCompletion_no_matches()
	{
		// FIXME: Rate-limit?
		update_locationCompletion();
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
			try {
				cvn::fs::Stat loc_stat(location_str_);
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
			catch (const std::exception &ex) {
				display_errmsg(
					std::string("Couldn't activate ls TreeView row: ")
					+ "Couldn't get stat information for current location: "
					+ ex.what());
			}
		}
	}

	void LsGui::on_ls_column_clicked(int columnNr)
	{
		int prev_columnNr = 0;
		Gtk::SortType prev_order = Gtk::SortType::SORT_ASCENDING;
		if (modelSort_->get_sort_column_id(prev_columnNr, prev_order))
		{
			Gtk::SortType new_order = prev_order;
			if (prev_columnNr == columnNr) {
				// Switch sort order.
				switch (prev_order) {
				case Gtk::SortType::SORT_ASCENDING:
					new_order = Gtk::SortType::SORT_DESCENDING;
					break;
				case Gtk::SortType::SORT_DESCENDING:
					new_order = Gtk::SortType::SORT_ASCENDING;
					break;
				default:
					// Leave it as-is, but warn.
					g_warning("Unrecognized previous sort order '%d'!", prev_order);
					break;
				}
			}

			modelSort_->set_sort_column(columnNr, new_order);
		}
		else {
			modelSort_->set_sort_column(columnNr, Gtk::SortType::SORT_ASCENDING);
		}
	}

	void LsGui::on_action_open()
	{
		auto dialog = Gtk::FileChooserDialog(
			*this, "Open directory - ls",
			Gtk::FileChooserAction::FILE_CHOOSER_ACTION_SELECT_FOLDER);
		dialog.set_show_hidden(get_show_hidden());
		dialog.set_create_folders(false);
		dialog.add_button("_Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
		dialog.add_button("_Open", Gtk::ResponseType::RESPONSE_OK);

		int result = dialog.run();
		switch (result) {
		case Gtk::ResponseType::RESPONSE_OK:
			set_location_str(Glib::filename_to_utf8(dialog.get_filename()));
			break;
		default:
			return;
		}
	}

	void LsGui::on_action_close()
	{
		// Just clean up current state, for now.
		// (Will need change when introducing tab pages.)
		history_clear();
		set_location_str();
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
		std::cout << "Toggling show hidden to "
		          << show_hidden_new << "..." << std::endl;
		action_show_hidden_ptr_->change_state(show_hidden_new);

		if (location_is_dirlisting_) {
			// Re-read directory with new state.
			set_location_str();
		}
	}

	void LsGui::on_action_complete_location()
	{
		// Toggle state of the action.
		bool complete_location_new = !get_complete_location();
		std::cout << "Toggling complete location to "
		          << complete_location_new << "..." << std::endl;
		action_complete_location_ptr_->change_state(complete_location_new);

		location_.set_completion(complete_location_new ?
			locationCompletion_ptr_ : Glib::RefPtr<Gtk::EntryCompletion>());
	}

}  // cvn::lsgui
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6
