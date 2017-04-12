#include "entrycvn.hh"

EntryCvn::EntryCvn() :
	outerBox_(Gtk::ORIENTATION_VERTICAL),
	middleInnerBox_(Gtk::ORIENTATION_HORIZONTAL),
	editableCheckButton_("_Editable", true),
	visibleCheckButton_("_Visible", true),
	closeButton_("_Close", true)
{
	set_title("entry test by canvon");
	entry_.set_text("hello world");

	add(outerBox_);

	outerBox_.pack_start(entry_);
	outerBox_.pack_start(middleInnerBox_);
	outerBox_.pack_start(closeButton_);

	middleInnerBox_.pack_start(editableCheckButton_);
	middleInnerBox_.pack_start(visibleCheckButton_);

	entry_.set_activates_default();
	closeButton_.set_can_default();
	closeButton_.grab_default();

	editableCheckButton_.signal_toggled().connect(
		sigc::mem_fun(*this, &EntryCvn::on_editable_toggled));
	visibleCheckButton_.signal_toggled().connect(
		sigc::mem_fun(*this, &EntryCvn::on_visible_toggled));
	closeButton_.signal_clicked().connect(
		sigc::mem_fun(*this, &EntryCvn::on_close_clicked));

	show_all_children();
}

EntryCvn::~EntryCvn()
{
}

void EntryCvn::on_editable_toggled()
{
	entry_.set_editable(editableCheckButton_.get_active());
}

void EntryCvn::on_visible_toggled()
{
	entry_.set_visible(visibleCheckButton_.get_active());
}

void EntryCvn::on_close_clicked()
{
	hide();
}
