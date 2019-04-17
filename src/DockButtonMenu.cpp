#include "DockButtonMenu.hpp"

#include "DockButton.hpp"
#include "DockButtonMenuItem.hpp"

#include "Plugin.hpp"

DockButtonMenu::DockButtonMenu(DockButton* dockButton)
{
	mWindow = gtk_window_new(GtkWindowType::GTK_WINDOW_POPUP);
	//gtk_style_context_add_class(gtk_widget_get_style_context(mWindow), "popup");
	gtk_window_set_default_size(GTK_WINDOW(mWindow), 1, 1);
	mDockButton = dockButton;

	mMouseHover = false;

	mBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(mWindow), mBox);
	gtk_widget_show(mBox);

	g_signal_connect(G_OBJECT(mWindow), "enter-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, DockButtonMenu* me){
		me->mMouseHover = true;
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mWindow), "leave-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, DockButtonMenu* me){
		int w; int h; gtk_window_get_size(GTK_WINDOW(me->mWindow), &w, &h);
		int mx = ((GdkEventCrossing*)event)->x; int my = ((GdkEventCrossing*)event)->y;
		if(mx >= 0 && mx < w && my >= 0 && my < h) return true;

		me->mDockButton->setMouseLeaveTimeout();
		me->mMouseHover = false;
		return true;
	}), this);
}

void DockButtonMenu::add(DockButtonMenuItem* menuItem)
{
	gtk_box_pack_start(GTK_BOX(mBox), menuItem->mTitleButton, false, true, 0);
}

void DockButtonMenu::remove(DockButtonMenuItem* menuItem)
{
	gtk_container_remove(GTK_CONTAINER(mBox), menuItem->mTitleButton);
	gtk_window_resize(GTK_WINDOW(mWindow), 1, 1);
}

void DockButtonMenu::popup()
{
	gint wx, wy;
	xfce_panel_plugin_position_widget(Plugin::mXfPlugin, mWindow, mDockButton->mButton, &wx, &wy);

	gtk_window_move(GTK_WINDOW(mWindow), wx, wy);
	gtk_widget_show(mWindow);

	gtk_window_resize(GTK_WINDOW(mWindow), 1, 1);
}

void DockButtonMenu::hide()
{
	gtk_widget_hide(mWindow);
}