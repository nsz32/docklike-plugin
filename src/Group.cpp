// ** opensource.org/licenses/GPL-3.0

#include "Group.hpp"

Group::Group(std::string groupName, AppInfo* appInfo, bool pinned)
{
	mGroupName = groupName;
	mAppInfo = appInfo;
	mPinned = pinned;

	if(mPinned) show();	

	set_data("groupObject", this);

	set_tooltip_text(appInfo->name);

	add_events(Gdk::EventMask::SCROLL_MASK);
	set_always_show_image(true);

	signal_button_press_event().connect(sigc::mem_fun(*this, &Group::onButtonPress));
	signal_button_release_event().connect(sigc::mem_fun(*this, &Group::onButtonRelease));
	signal_scroll_event().connect(sigc::mem_fun(*this, &Group::onMouseScroll));
	signal_drag_begin().connect(sigc::mem_fun(*this, &Group::onDragBegin));
	signal_drag_motion().connect(sigc::mem_fun(*this, &Group::onDragMotion));
	signal_drag_leave().connect(sigc::mem_fun(*this, &Group::onDragLeave));
	signal_drag_data_get().connect(sigc::mem_fun(*this, &Group::onDragDataGet));
	signal_drag_data_received().connect(sigc::mem_fun(*this, &Group::onDragDataReceived));

	if(mAppInfo != NULL && !mAppInfo->icon.empty())
	{
			std::cout << "LOADIMAGE:" << mAppInfo->icon << std::endl;
		if(mAppInfo->icon [0] == '/')
		{
			//set_image_from_icon_name(mAppInfo->icon);

			/* TODO RESIZE : Gtk::Image* z = new Gtk::Image(ai->icon);
			z->set_pixel_size(16);

			set_image(*z);*/
		}
		else
		{
			
			set_image_from_icon_name(mAppInfo->icon);
		}
	}
	else
	{
		set_image_from_icon_name("application-x-executable");
	}

	Gtk::TargetEntry te;
	te.set_target("NMT/Group");

	std::vector<Gtk::TargetEntry> v;
	v.push_back(te);
	Glib::RefPtr<Gtk::TargetList> tl = Gtk::TargetList::create(v);

	drag_source_set(v, Gdk::ModifierType::BUTTON1_MASK, Gdk::DragAction::ACTION_MOVE);

	drag_dest_set(v, Gtk::DestDefaults::DEST_DEFAULT_ALL, Gdk::DragAction::ACTION_MOVE);
}

void Group::addWindow(GroupWindow* window)
{
	mWindows.push(window->getXID(), window);

	get_style_context()->add_class("opened");
}

void Group::removeWindow(gulong XID)
{
	GroupWindow* window = mWindows.pop(XID);
	delete window;

	if(!hasWindows())
		get_style_context()->remove_class("opened");

	updateVisibility();
}

void Group::updateVisibility()
{
	if(mPinned)
		show();
	else if(hasWindows())
		show();
	else
		hide();
}

bool Group::onButtonPress(GdkEventButton* event)
{

	std::cout << "button:" << event->button << std::endl;

	if(event->state & GDK_SHIFT_MASK)
	{
		/* send the event to the panel plugin 
		panel_plugin = xfce_tasklist_get_panel_plugin (child->tasklist);
		if (G_LIKELY (panel_plugin != NULL))
		gtk_widget_event (panel_plugin, (GdkEvent *) event);*/

		return true;
	}
	
	if(!hasWindows() || event->button != 3) return true;
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
				me->updateVisibility();
			Taskbar::savePinned();
			
		}), this);

		gtk_menu_attach_to_widget (GTK_MENU (menu), GTK_WIDGET(this->gobj()), NULL);

		gtk_menu_popup_at_widget (GTK_MENU (menu), GTK_WIDGET(this->gobj()), GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, (GdkEvent *) event);

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

	if(event->state & GDK_SHIFT_MASK || (mPinned && hasWindows() == false))
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

bool Group::onMouseScroll(GdkEventScroll* event)
{
	if(mPinned && !hasWindows()) return true;

	if(mWindows.size() == 1)
		return true;

	guint32 timestamp = event->time;

	if(!mActive)
	{
		mWindows.last()->activate(timestamp);
		return true;
	}

	if(event->direction == GDK_SCROLL_DOWN)
	{
		mWindows.shiftToBack();
		mWindows.last()->activate(timestamp);
	}
	else if(event->direction == GDK_SCROLL_UP)
	{
		mWindows.shiftToFront();
		mWindows.last()->activate(timestamp);
	}

	return true;
}

void Group::onDragBegin(const Glib::RefPtr<Gdk::DragContext>& context)
{
	gtk_drag_set_icon_name(context->gobj(), mAppInfo->icon.c_str(),0,0);
}

bool Group::onDragMotion(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
	get_style_context()->add_class("drop");
	return true;
}

void Group::onDragLeave(const Glib::RefPtr<Gdk::DragContext>& context, guint time)
{
	get_style_context()->remove_class("drop");
}

void Group::onDragDataGet(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time)
{
	Group* me = this;
	std::cout << "pme:" << me << std::endl;

	//TODO is the source object copied or passed by a pointer ?
	selection_data.set("button", 32, (const guchar*)me, sizeof (gpointer)*32);
}

void Group::onDragDataReceived(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
	std::string dt = selection_data.get_data_type();
	if(dt != "button") return;

	int s = 0;
	Group* source = (Group*)selection_data.get_data(s);

	std::cout << "psource:" << source << std::endl;

	Taskbar::moveButton(source, this);
}

void Group::onWindowActivate(gulong XID)
{
	GroupWindow* window = mWindows.moveBack(XID);

	mActive = true;
	get_style_context()->add_class("active");
}

void Group::onWindowUnactivate()
{
	mActive = false;
	get_style_context()->remove_class("active");
}

bool Group::hasWindows()
{
	GroupWindow* window = mWindows.findIf([](std::pair<gulong, GroupWindow*> r)->bool
		{ return !r.second->getState(WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST); });
	
	return window != NULL;
}
