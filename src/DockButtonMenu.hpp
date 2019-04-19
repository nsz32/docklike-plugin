#ifndef DOCK_BUTTON_MENU_HPP
#define DOCK_BUTTON_MENU_HPP

#include <iostream>

#include <gtk/gtk.h>



class DockButton;
class DockButtonMenuItem;

class DockButtonMenu
{
	public:
		DockButtonMenu(DockButton* dockButton);

		void add(DockButtonMenuItem& menuItem);
		void remove(DockButtonMenuItem& menuItem);

		void popup();
		void hide();

		DockButton* mDockButton;
		
		GtkWidget* mWindow;
		GtkWidget* mBox;

		bool mMouseHover;
};

#endif 