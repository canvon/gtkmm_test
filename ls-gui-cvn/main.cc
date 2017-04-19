#include "lsgui.hh"
#include <gtkmm/application.h>

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "de.canvon.gtkmm-test.ls");

	LsGui window;

	app->add_action("quit", [&window] {
		// TODO: This needs to be extended when we implement multi-window mode.
		window.hide();
	});

	return app->run(window);
}
