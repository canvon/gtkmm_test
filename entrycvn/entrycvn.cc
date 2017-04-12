#include "entrycvn.hh"

EntryCvn::EntryCvn() :
	outerBox_(Gtk::ORIENTATION_VERTICAL),
	middleInnerBox_(Gtk::ORIENTATION_HORIZONTAL),
	editableCheckButton_("_Editable"),
	visibleCheckButton_("_Visible"),
	closeButton_("_Close")
{
	set_title("entry test by canvon");
	entry_.set_text("hello world");

	add(outerBox_);

	outerBox_.pack_start(entry_);
	outerBox_.pack_start(middleInnerBox_);
	outerBox_.pack_start(closeButton_);

	middleInnerBox_.pack_start(editableCheckButton_);
	middleInnerBox_.pack_start(visibleCheckButton_);

	entry_.set_can_default();
	entry_.grab_default();

	editableCheckButton_.signal_toggled().connect(
		sigc::mem_fun(*this, &EntryCvn::on_editable_toggled));
	visibleCheckButton_.signal_toggled().connect(
		sigc::mem_fun(*this, &EntryCvn::on_visible_toggled));
	closeButton_.signal_clicked().connect(
		sigc::mem_fun(*this, &EntryCvn::on_close_clicked));
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
