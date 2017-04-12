#include "entrycvn.hh"
#include <gtkmm/application.h>

int main(int argc, char *argv[])
{
	auto app = Gtk::Application::create(argc, argv, "de.canvon.gtkmm-test.entrycvn");

	EntryCvn window;

	return app->run(window);
}
