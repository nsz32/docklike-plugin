#include "GroupMenuItem.hpp"

#include "GroupWindow.hpp"

GroupMenuItem::GroupMenuItem(GroupWindow* groupWindow)
{
	mGroupWindow = groupWindow;

	mItem = (GtkEventBox*)gtk_event_box_new();
	Help::Gtk::cssClassAdd(GTK_WIDGET(mItem), "menu_item");
	gtk_widget_show(GTK_WIDGET(mItem));

	mGrid = (GtkGrid*)gtk_grid_new();
	gtk_widget_show(GTK_WIDGET(mGrid));
	gtk_container_add(GTK_CONTAINER(mItem), GTK_WIDGET(mGrid));

	mIcon = (GtkImage*)gtk_image_new();
	gtk_widget_show(GTK_WIDGET(mIcon));
	gtk_grid_attach(mGrid, GTK_WIDGET(mIcon), 0, 0, 1, 1);

	mLabel = (GtkLabel*)gtk_label_new("");
	gtk_label_set_xalign(mLabel, 0);
	gtk_label_set_ellipsize(mLabel, PANGO_ELLIPSIZE_END);
	gtk_label_set_width_chars(mLabel, 26);
	gtk_widget_show(GTK_WIDGET(mLabel));
	gtk_grid_attach(mGrid, GTK_WIDGET(mLabel), 1, 0, 1, 1);

	mCloseButton = (GtkButton*)gtk_button_new_with_label("⨯");
	gtk_widget_show(GTK_WIDGET(mCloseButton));
	gtk_grid_attach(mGrid, GTK_WIDGET(mCloseButton), 2, 0, 1, 1);

	g_signal_connect(G_OBJECT(mItem), "button-press-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventButton* event, GroupMenuItem* me){
		gdk_device_ungrab((event)->device, (event)->time);
		me->mGroupWindow->activate((event)->time);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mItem), "enter-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventCrossing* event, GroupMenuItem* me){
		Help::Gtk::cssClassAdd(GTK_WIDGET(me->mItem), "hover");
		if(event->state & GDK_BUTTON1_MASK)
			me->mGroupWindow->activate(event->time);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mItem), "leave-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, GroupMenuItem* me){
		Help::Gtk::cssClassRemove(GTK_WIDGET(me->mItem), "hover");
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mCloseButton), "clicked",
	G_CALLBACK(+[](GtkButton* button, GroupMenuItem* me){
		Wnck::close(me->mGroupWindow, 0);
	}), this);
}

void GroupMenuItem::updateLabel()
{
	gtk_label_set_text(mLabel, Wnck::getName(mGroupWindow).c_str());
}

void GroupMenuItem::updateIcon()
{
	GdkPixbuf* iconPixbuf = Wnck::getMiniIcon(mGroupWindow);
	if(iconPixbuf != NULL)
		gtk_image_set_from_pixbuf(GTK_IMAGE(mIcon), iconPixbuf);
}