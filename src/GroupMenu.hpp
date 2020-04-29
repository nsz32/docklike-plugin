/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef DOCK_BUTTON_MENU_HPP
#define DOCK_BUTTON_MENU_HPP

#include <gtk/gtk.h>

#include <iostream>

class Group;
class GroupMenuItem;

class GroupMenu
{
  public:
	GroupMenu(Group* dockButton);

	void add(GroupMenuItem* menuItem);
	void remove(GroupMenuItem* menuItem);

	void popup();
	void hide();

	uint getPointerDistance();

	Group* mGroup;

	GtkWidget* mWindow;
	GtkWidget* mBox;

	bool mVisible;
	bool mMouseHover;
};

#endif