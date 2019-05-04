#ifndef DOCK_BUTTON_MENU_HPP
#define DOCK_BUTTON_MENU_HPP

#include <iostream>

#include <gtk/gtk.h>


class Group;
class GroupMenuItem;

class GroupMenu
{
	public:
		GroupMenu(Group* dockButton);

		void add(GroupMenuItem& menuItem);
		void remove(GroupMenuItem& menuItem);

		void popup();
		void hide();

		uint getPointerDistance();

		Group* mGroup;
		
		GtkWidget* mWindow;
		GtkWidget* mBox;

		bool mMouseHover;
};

#endif 