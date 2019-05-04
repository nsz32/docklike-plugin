#ifndef DOCK_BUTTON_MENU_ITEM_HPP
#define DOCK_BUTTON_MENU_ITEM_HPP

#include <iostream>

#include <gtk/gtk.h>



class GroupWindow;

class GroupMenuItem
{
	public:
		GroupMenuItem(GroupWindow* groupWindow);

		void updateLabel();
		void updateIcon();

		GroupWindow* mGroupWindow;
		
		GtkEventBox* mItem;
		GtkGrid* mGrid;
		GtkImage* mIcon;
		GtkLabel* mLabel;
		GtkButton* mCloseButton;
};

#endif 