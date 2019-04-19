#include "DockButtonMenuItem.hpp"

#include "GroupWindow.hpp"

DockButtonMenuItem::DockButtonMenuItem(GroupWindow* groupWindow)
{
	mGroupWindow = groupWindow;

	mTitleButton = gtk_button_new_with_label("_");
	gtk_style_context_add_class(gtk_widget_get_style_context(mTitleButton), "docklike_menu");
	gtk_widget_show(mTitleButton);

	gtk_widget_set_focus_on_click(GTK_WIDGET(mTitleButton), false);
	gtk_widget_set_can_focus(GTK_WIDGET(mTitleButton), false);
	gtk_widget_set_can_default(GTK_WIDGET(mTitleButton), false);
	
	GtkLabel* label = (GtkLabel*)gtk_bin_get_child(GTK_BIN(mTitleButton));
	gtk_label_set_xalign(label, 0);
	gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
	gtk_label_set_width_chars(label, 32);
	gtk_widget_set_hexpand(GTK_WIDGET(label), true);



	g_signal_connect(G_OBJECT(mTitleButton), "button-press-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventButton* event, DockButtonMenuItem* me){
		gdk_device_ungrab((event)->device, (event)->time);
		me->mGroupWindow->activate((event)->time);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mTitleButton), "enter-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventCrossing* event, DockButtonMenuItem* me){
		if(event->state & GDK_BUTTON1_MASK)
			me->mGroupWindow->activate(event->time);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mTitleButton), "leave-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, DockButton* me){
		return true;
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