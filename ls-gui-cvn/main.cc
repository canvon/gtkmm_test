#define G_LOG_USE_STRUCTURED

#include "lsgui.hh"
#include <gtkmm/application.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/aboutdialog.h>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <typeinfo>

#include "versioncheck.hh"


#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "lsgui"

#ifdef HAVE_STRUCTURED_LOGGING
GLogWriterOutput ls_gui_log_writer(
	GLogLevelFlags log_level,
	const GLogField *fields,
	gsize n_fields,
	gpointer user_data)
{
	try {
		// (Think: priority)
		if ((log_level & G_LOG_LEVEL_MASK) < G_LOG_LEVEL_INFO) {
			Glib::ustring msg, domain;
			for (guint i = 0; i < n_fields; i++) {
				const GLogField &field(fields[i]);
				if (field.length >= 0)
					// Ignore binary data, for now.
					// (C strings will have -1 as length, it seems.)
					continue;

				auto key = std::string(field.key);
				auto value_cstr = static_cast<const char *>(field.value);
				if (key == "MESSAGE")
					msg = value_cstr;
				else if (key == "GLIB_DOMAIN")
					domain = value_cstr;
			}

			if (msg.empty())
				msg = "(message missing)";

			if (log_level & G_LOG_FATAL_MASK) {
				// Fatal errors get a message box
				// instead of the more user-friendly InfoBar handling.
				std::cerr
					<< "Launching a message box for fatal message"
					<< " from domain " << std::quoted(domain.raw())
					<< ": " << std::quoted(msg.raw()) << std::endl;
				auto dialog = Gtk::MessageDialog(
					"Fatal"
					+ (domain.empty() ? "" : ", from " + domain)
					+ ": " + msg,
					false,
					Gtk::MessageType::MESSAGE_ERROR,
					Gtk::ButtonsType::BUTTONS_OK,
					true);
				dialog.set_title("Fatal - ls");
				dialog.run();
			}
			else {
				do {
					if (!user_data) {
						std::cerr << "Error making glib log message visible to the user: "
							<< "Application instance is not available!" << std::endl;
						break;
					}

					Glib::RefPtr<Gtk::Application> &app(*static_cast<Glib::RefPtr<Gtk::Application> *>(user_data));

					auto windows_lst = app->get_windows();
					if (windows_lst.empty()) {
						std::cerr << "Error making glib log message visible to the user: "
							<< "List of application windows is empty!" << std::endl;
						break;
					}

					auto window_ptr = dynamic_cast<cvn::lsgui::LsGui *>(windows_lst[0]);
					if (!window_ptr) {
						std::cerr << "Error making glib log message visible to the user: "
							<< "Window with focus is not an LsGui!" << std::endl;
						break;
					}

					window_ptr->display_glib_msg(domain, log_level, msg, fields);
				}
				while (false);
			}
		}
#if 0
		else {
			std::cout << "(Debug/Info message ignored.)" << std::endl;
		}
#endif
	}
	catch (const std::exception &ex) {
		std::cerr
			<< "Error: Caught exception of type " << typeid(ex).name()
			<< " while in glib log writer " << __func__
			<< " (ignoring): "
			<< ex.what() << std::endl;
	}

	// Pass logging data on to the default writer.
	return g_log_writer_default(log_level, fields, n_fields, user_data);
}
#endif

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "de.canvon.gtkmm-test.ls");

#ifdef HAVE_STRUCTURED_LOGGING
	// Set a glib log writer which will display to the user.
	g_log_set_writer_func(ls_gui_log_writer, &app, NULL);
#else
#pragma message("Warning: Won't set a log writer function, " REQUIRES_STRUCTURED_LOGGING)
#endif

	// The main window.
	cvn::lsgui::LsGui window;

#ifndef HAVE_STRUCTURED_LOGGING
	{
		Glib::ustring msg("Startup warning: Compiled without structured logging support; won't show glib log messages in the GUI!");
		g_warning("%s", msg.c_str());
		window.display_errmsg(msg);
	}
#endif

	app->signal_startup().connect([app, &window] {
		// Set application-wide menubar.
		//
		// This seems to be necessary to get the accelerators
		// picked up automatically from the menubar markup.
		app->set_menubar(window.get_gmenu());
	});

	app->add_action("about", [] {
		Gtk::AboutDialog dialog;
		// TODO: dialog.set_version();
		dialog.set_comments("An ls (list directory command) GUI by canvon");
		dialog.set_authors({ "Fabian Pietsch aka canvon <fabian@canvon.de>" });
		dialog.set_copyright("Copyright © 2017 Fabian Pietsch <fabian@canvon.de>");
		// TODO: dialog.set_license() or set_license_type();
		dialog.set_website("https://github.com/canvon/gtkmm_test");
		dialog.set_website_label("Website: gtkmm_test on GitHub");
		dialog.run();
	});

	app->add_action("quit", [&window] {
		std::cout << "Quit." << std::endl;

		// TODO: This needs to be extended when we implement multi-window mode.
		window.hide();
	});

	return app->run(window);
}
