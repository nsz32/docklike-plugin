// ** opensource.org/licenses/GPL-3.0

#include "Group.hpp"

static GtkTargetEntry entries[1] = { { "application/docklike_group", 0 ,0 } };

Group::Group(std::string groupName, AppInfo* appInfo, bool pinned): DockButton(pinned)
{
	mGroupName = groupName;
	mAppInfo = appInfo;
	mPinned = pinned;

	if(mPinned) gtk_widget_show(mButton);

	g_object_set_data(G_OBJECT(mButton),"group", this);

	gtk_button_set_relief(GTK_BUTTON(mButton), GTK_RELIEF_NONE);

	gtk_widget_add_events(mButton, GDK_SCROLL_MASK);
	gtk_button_set_always_show_image(GTK_BUTTON(mButton), true);


	g_signal_connect(G_OBJECT(mButton), "button-press-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, Group* me){
		std::cout << "press:" << 1 << std::endl;
		me->onButtonPress((GdkEventButton*)event);
	}), this);

	g_signal_connect(G_OBJECT(mButton), "button-release-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, Group* me){
		return me->onButtonRelease((GdkEventButton*)event);
	}), this);


	g_signal_connect(G_OBJECT(mButton), "scroll-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEvent* event, Group* me){
		return me->onScroll((GdkEventScroll*)event);
	}), this);


	g_signal_connect(G_OBJECT(mButton), "drag-begin",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, Group* me){
		std::cout << "dbegin:" << 1 << std::endl;
		me->onDragBegin(context);
	}), this);

	if(mAppInfo != NULL && !mAppInfo->icon.empty())
	{
		/*	std::cout << "NEW GROUP:" << mAppInfo->name << std::endl;
			std::cout << "PATH:" << mAppInfo->path << std::endl;
			std::cout << "ICON:" << mAppInfo->icon << std::endl << std::endl;*/
		if(mAppInfo->icon [0] == '/')
		{
			//set_image_from_icon_name(mAppInfo->icon);

			/* TODO RESIZE : Gtk::Image* z = new Gtk::Image(ai->icon);
			z->set_pixel_size(16);

			set_image(*z);*/
		}
		else
		{
			GtkWidget* icon = gtk_image_new_from_icon_name(mAppInfo->icon.c_str(), GTK_ICON_SIZE_BUTTON);
			gtk_button_set_image(GTK_BUTTON(mButton), icon);
		}
	}
	else
	{
		GtkWidget* icon = gtk_image_new_from_icon_name("application-x-executable", GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image(GTK_BUTTON(mButton), icon);
	}

	gtk_drag_source_set(mButton, GDK_BUTTON1_MASK, entries, 1, GDK_ACTION_MOVE);
	gtk_drag_dest_set(mButton, GTK_DEST_DEFAULT_DROP, entries, 1, GDK_ACTION_MOVE);

	resize();
}

void Group::addWindow(GroupWindow* window)
{
	mWindows.push(window->getXID(), window);
	mDockButtonMenu.add(&(window->mDockButtonMenuItem));

	updateStyle();
}

void Group::removeWindow(gulong XID)
{
	GroupWindow* window = mWindows.pop(XID);
	window->mGroup->mDockButtonMenu.remove(&(window->mDockButtonMenuItem));
	delete window;

	updateStyle();
}

void Group::updateStyle()
{
	int wCount = hasVisibleWindows();

	if(mPinned || wCount)
		gtk_widget_show(mButton);
	else
		gtk_widget_hide(mButton);

	if(wCount)
		setStyle(Style::Opened, true);
	else
		setStyle(Style::Opened, false);

	if(wCount > 1)
		setStyle(Style::Many, true);
	else
		setStyle(Style::Many, false);
	
}

int Group::hasVisibleWindows()
{
	int count = 0;

	//std::cout << "CNT:" << mWindows.size() << std::endl;

	mWindows.findIf([&count](std::pair<gulong, GroupWindow*> e)->bool
		{
			if(!e.second->getState(WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST))
			{
				++count;
				if(count == 2) return true;
			}
			return false;
		});
	
	return count;
}

void Group::onWindowActivate(gulong XID)
{
	GroupWindow* window = mWindows.moveBack(XID);

	mActive = true;
	setStyle(Style::Focus, true);
}

void Group::onWindowUnactivate()
{
	mActive = false;
	setStyle(Style::Focus, false);
}


bool Group::onButtonPress(GdkEventButton* event)
{

	if(event->button != 3 || event->state & GDK_CONTROL_MASK)
		return false;
	
	if(!hasVisibleWindows() || event->button != 3) return true;
	else
	{
		GtkWidget* menu = Wnck::getActionMenu(mWindows.last()->mWnckWindow);

		GtkWidget* launchAnother = gtk_menu_item_new_with_label("Launch another");
		GtkWidget* separator = gtk_separator_menu_item_new();
		GtkWidget* pinToggle = mPinned ? 
			gtk_menu_item_new_with_label("Unpin") :
			gtk_menu_item_new_with_label("Pin this app");

		gtk_widget_show(separator);
		gtk_widget_show(launchAnother);
		gtk_widget_show(pinToggle);

		gtk_menu_attach(GTK_MENU (menu), GTK_WIDGET(launchAnother), 0, 1, 0, 1);
		gtk_menu_attach(GTK_MENU (menu), GTK_WIDGET(separator), 1, 2, 0, 2);
		gtk_menu_attach(GTK_MENU (menu), GTK_WIDGET(pinToggle), 1, 2, 0, 2);

		g_signal_connect(G_OBJECT(launchAnother), "activate",
		G_CALLBACK(+[](GtkMenuItem *menuitem, Group* me)
		{
			AppInfos::launch(me->mAppInfo);
		}), this);

		g_signal_connect(G_OBJECT(pinToggle), "activate",
		G_CALLBACK(+[](GtkMenuItem *menuitem, Group* me)
		{
			me->mPinned = !me->mPinned;
			if(!me->mPinned)
				me->updateStyle();
			Dock::savePinned();
			
		}), this);

		gtk_menu_attach_to_widget (GTK_MENU (menu), GTK_WIDGET(mButton), NULL);

		gtk_menu_popup_at_widget (GTK_MENU (menu), GTK_WIDGET(mButton), GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, (GdkEvent *) event);

		//then destroy TODO
		/* g_signal_connect (G_OBJECT (menu), "selection-done",
          G_CALLBACK (xfce_tasklist_button_menu_destroy), child);

					static void
			xfce_tasklist_button_menu_destroy (GtkWidget         *menu,
											XfceTasklistChild *child)
			{
			panel_return_if_fail (XFCE_IS_TASKLIST (child->tasklist));
			panel_return_if_fail (GTK_IS_TOGGLE_BUTTON (child->button));
			panel_return_if_fail (GTK_IS_WIDGET (menu));

			gtk_widget_destroy (menu);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (child->button), FALSE);
			}*/
	}

	return true;
}

bool Group::onButtonRelease(GdkEventButton* event)
{
	std::cout << "RELEASE:" << 1 << std::endl;
	if(event->button != 1) return true;

	if(event->state & GDK_SHIFT_MASK || (mPinned && !hasVisibleWindows()))
	{
		AppInfos::launch(mAppInfo);
	}
	else if(mActive)
	{
		mWindows.last()->minimize();
	}
	else
	{
		guint32 timestamp = event->time;
		mWindows.forEach([&timestamp](std::pair<gulong, GroupWindow*> w)->void { w.second->activate(timestamp); });
	}

	return true;
}

bool Group::onScroll(GdkEventScroll* event)
{
	if(mPinned && !hasVisibleWindows()) return true;

	if(mWindows.size() == 1)
		return true;

	if(!mActive)
	{
		mWindows.last()->activate(event->time);
		return true;
	}

	if(event->direction == GDK_SCROLL_DOWN)
	{
		mWindows.shiftToBack();
		mWindows.last()->activate(event->time);
	}
	else if(event->direction == GDK_SCROLL_UP)
	{
		mWindows.shiftToFront();
		mWindows.last()->activate(event->time);
	}

	return true;
}

void Group::onDragBegin(GdkDragContext* context)
{
	gtk_drag_set_icon_name(context, mAppInfo->icon.c_str(),0,0);
}