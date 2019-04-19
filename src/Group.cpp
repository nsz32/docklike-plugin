// ** opensource.org/licenses/GPL-3.0

#include "Group.hpp"

Group::Group(AppInfo* appInfo, bool pinned): DockButton(pinned)
{
	mAppInfo = appInfo;
	mPinned = pinned;
	mActive = false;

	mTopWindowIndex = 0;

	if(mPinned) gtk_widget_show(mButton);

	g_object_set_data(G_OBJECT(mButton),"group", this);

	gtk_button_set_relief(GTK_BUTTON(mButton), GTK_RELIEF_NONE);

	gtk_widget_add_events(mButton, GDK_SCROLL_MASK);
	gtk_button_set_always_show_image(GTK_BUTTON(mButton), true);


	g_signal_connect(G_OBJECT(mButton), "button-press-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventButton* event, Group* me){
		if(event->button != 3 || event->state & GDK_CONTROL_MASK) return false;
		me->onButtonPress(event);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "button-release-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventButton* event, Group* me){
		if(event->button != 1) return false;
		me->onButtonRelease(event);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "scroll-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventScroll* event, Group* me){
		me->onScroll((GdkEventScroll*)event);
		return true;
	}), this);


	g_signal_connect(G_OBJECT(mButton), "drag-begin",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, Group* me){
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

void Group::electNewTopWindow()
{
	if(mWindows.size() > 0)
	{
		GroupWindow* newTopWindow;

		if(mWindows.size() == 1)
			newTopWindow = mWindows.get(0);
		else
		{
			newTopWindow = Wnck::mGroupWindows.findIf([this](std::pair<gulong, GroupWindow*> e)->bool
			{
				if(e.second->mGroup == this) return true;
				return false;
			});
		}

		setTopWindow(newTopWindow);
	}
}

int Group::hasVisibleWindows()
{
	int count = 0;

	mWindows.findIf([&count](GroupWindow* e)->bool
	{
		if(!e->getState(WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST))
		{
			++count;
			if(count == 2) return true;
		}
		return false;
	});
	
	return count;
}

void Group::onWindowActivate(GroupWindow* groupWindow)
{
	mActive = true;
	setStyle(Style::Focus, true);

	setTopWindow(groupWindow);
}

void Group::onWindowUnactivate()
{
	mActive = false;
	setStyle(Style::Focus, false);
}

void Group::setTopWindow(GroupWindow* groupWindow)
{
	mWindows.forEach([](GroupWindow* gW)->void { Help::Gtk::cssClassRemove(gW->mDockButtonMenuItem.mTitleButton, "top"); });
	Help::Gtk::cssClassAdd(groupWindow->mDockButtonMenuItem.mTitleButton, "top");

	mTopWindowIndex = mWindows.getIndex(groupWindow);
}

void Group::onButtonPress(GdkEventButton* event)
{
	std::cout << "PRESS MENU HERE:" << 1 << std::endl;
	
	if(!hasVisibleWindows() || event->button != 3) return;
	else
	{
		/*GtkWidget* menu = Wnck::getActionMenu(mWindows.last()->mWnckWindow);

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

		gtk_menu_popup_at_widget (GTK_MENU (menu), GTK_WIDGET(mButton), GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, (GdkEvent *) event);*/

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
}

void Group::onButtonRelease(GdkEventButton* event)
{
	if(event->state & GDK_SHIFT_MASK || (mPinned && !hasVisibleWindows()))
	{
		AppInfos::launch(mAppInfo);
	}
	else if(mActive)
	{
		mWindows.get(mTopWindowIndex)->minimize();
	}
	else
	{
		guint32 timestamp = event->time;
		GroupWindow* groupWindow = mWindows.get(mTopWindowIndex);

		mWindows.forEach([&timestamp, &groupWindow](GroupWindow* w)->void
		{
			if(w != groupWindow) w->activate(timestamp);
		});

		groupWindow->activate(timestamp);
	}
}

void Group::onScroll(GdkEventScroll* event)
{
	if(mPinned && !hasVisibleWindows()) return;

	if(!mActive)
	{
		mWindows.get(mTopWindowIndex)->activate(event->time);
	}
	else
	{
		if(event->direction == GDK_SCROLL_UP)
		mTopWindowIndex = ++mTopWindowIndex % mWindows.size();
		else if(event->direction == GDK_SCROLL_DOWN)
		{
			int size = mWindows.size();
			mTopWindowIndex = (--mTopWindowIndex + size) % size;
		}
		mWindows.get(mTopWindowIndex)->activate(event->time);
	}
}

void Group::onDragBegin(GdkDragContext* context)
{
	gtk_drag_set_icon_name(context, mAppInfo->icon.c_str(), 0, 0);
}