#include "DockButtonMenuItem.hpp"

#include "GroupWindow.hpp"

DockButtonMenuItem::DockButtonMenuItem(GroupWindow* groupWindow)
{
	mGroupWindow = groupWindow;

	mTitleButton = gtk_button_new_with_label("_");
	gtk_style_context_add_class(gtk_widget_get_style_context(mTitleButton), "docklike_menu");
	gtk_widget_show(mTitleButton);
	
	GtkLabel* label = (GtkLabel*)gtk_bin_get_child(GTK_BIN(mTitleButton));
	gtk_label_set_xalign(label, 0);
	gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
	gtk_label_set_width_chars(label, 32);
	gtk_widget_set_hexpand(GTK_WIDGET(label), true);

	g_signal_connect(G_OBJECT(mTitleButton), "button-press-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, DockButtonMenuItem* me){
		me->mGroupWindow->activate(((GdkEventButton*)event)->time);
	}), this);
}

void DockButtonMenuItem::setLabel(std::string title)
{
	gtk_button_set_label(GTK_BUTTON(mTitleButton), title.c_str());

	GtkLabel* label = (GtkLabel*)gtk_bin_get_child(GTK_BIN(mTitleButton));
	gtk_label_set_xalign(label, 0);
	gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
	gtk_label_set_width_chars(label, 32);
	gtk_widget_set_hexpand(GTK_WIDGET(label), true);
}