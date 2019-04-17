#ifndef DOCK_BUTTON_MENU_ITEM_HPP
#define DOCK_BUTTON_MENU_ITEM_HPP

#include <iostream>

#include <gtk/gtk.h>



class GroupWindow;

class DockButtonMenuItem
{
	public:
		DockButtonMenuItem(GroupWindow* groupWindow);

		void setLabel(std::string title);

		GtkWidget* mTitleButton;
		GroupWindow* mGroupWindow;
};

#endif 