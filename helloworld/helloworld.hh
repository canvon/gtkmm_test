#ifndef GTKMM_EXAMPLE_HELLOWORLD_HH
#define GTKMM_EXAMPLE_HELLOWORLD_HH

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class HelloWorld : public Gtk::Window
{
public:
	HelloWorld();
	virtual ~HelloWorld();

protected:
	// Signal handlers:
	void on_button_clicked();

	// Member widgets:
	Gtk::Button m_button;
};

#endif  // GTKMM_EXAMPLE_HELLOWORLD_HH
