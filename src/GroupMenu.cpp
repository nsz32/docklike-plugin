/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "GroupMenu.hpp"

#include "Group.hpp"
#include "GroupMenuItem.hpp"
#include "Plugin.hpp"

GroupMenu::GroupMenu(Group* dockButton)
{
	mWindow = gtk_window_new(GtkWindowType::GTK_WINDOW_POPUP);
	gtk_widget_add_events(mWindow, GDK_SCROLL_MASK);
	gtk_window_set_default_size(GTK_WINDOW(mWindow), 1, 1);
	gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(mWindow)), "stld");
	mGroup = dockButton;

	mVisible = mMouseHover = false;

	mBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	Help::Gtk::cssClassAdd(GTK_WIDGET(mBox), "menu");
	gtk_container_add(GTK_CONTAINER(mWindow), mBox);
	gtk_widget_show(mBox);

	//--------------------------------------------------

	g_signal_connect(G_OBJECT(mWindow), "enter-notify-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, GroupMenu* me) {
			me->mGroup->setStyle(Group::Style::Hover, true);
			me->mGroup->mLeaveTimeout.stop();
			me->mMouseHover = true;
			return true;
		}),
		this);

	g_signal_connect(G_OBJECT(mWindow), "leave-notify-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, GroupMenu* me) {
			int w;
			int h;
			gtk_window_get_size(GTK_WINDOW(me->mWindow), &w, &h);
			int mx = ((GdkEventCrossing*)event)->x;
			int my = ((GdkEventCrossing*)event)->y;
			if (mx >= 0 && mx < w && my >= 0 && my < h)
				return true;

			me->mGroup->setMouseLeaveTimeout();
			me->mMouseHover = false;
			return true;
		}),
		this);

	g_signal_connect(G_OBJECT(mWindow), "scroll-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEventScroll* event, GroupMenu* me) {
			((Group*)me->mGroup)->scrollWindows(event->time, event->direction);
			return true;
		}),
		this);
}

void GroupMenu::add(GroupMenuItem* menuItem)
{
	gtk_box_pack_end(GTK_BOX(mBox), GTK_WIDGET(menuItem->mItem), false, true, 0);

	if (mGroup->mSHover)
	{
		popup();
	}
}

void GroupMenu::remove(GroupMenuItem* menuItem)
{
	gtk_container_remove(GTK_CONTAINER(mBox), GTK_WIDGET(menuItem->mItem));
	gtk_window_resize(GTK_WINDOW(mWindow), 1, 1);

	if (mGroup->mWindowsCount < (Settings::noWindowsListIfSingle ? 2 : 1))
		gtk_widget_hide(mWindow);
}

void GroupMenu::popup()
{
	if (mGroup->mWindowsCount >= (Settings::noWindowsListIfSingle ? 2 : 1))
	{
		gint wx, wy;

		xfce_panel_plugin_position_widget(Plugin::mXfPlugin, mWindow, mGroup->mButton, &wx, &wy);
		gtk_window_move(GTK_WINDOW(mWindow), wx, wy);
		//xfce_panel_plugin_block_autohide(Plugin::mXfPlugin, true);
		gtk_widget_show(mWindow);

		gtk_window_resize(GTK_WINDOW(mWindow), 1, 1);

		mVisible = true;
	}
}

void GroupMenu::hide()
{
	gtk_widget_hide(mWindow);
	mVisible = false;
}

uint GroupMenu::getPointerDistance()
{
	gint wx, wy, ww, wh, px, py;

	gtk_window_get_position(GTK_WINDOW(mWindow), &wx, &wy);
	gtk_window_get_size(GTK_WINDOW(mWindow), &ww, &wh);

	Plugin::getPointerPosition(&px, &py);

	uint dx, dy;
	dx = dy = 0;

	if (px < wx)
		dx = wx - px;
	else if (px > wx + ww)
		dx = px - (wx + ww);

	if (py < wy)
		dy = wy - py;
	else if (py > wy + wh)
		dy = py - (wy + wh);

	return std::max(dx, dy);
}