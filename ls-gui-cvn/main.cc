#include "lsgui.hh"
#include <gtkmm/application.h>

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "de.canvon.gtkmm-test.ls");

	LsGui window;

	return app->run(window);
}
