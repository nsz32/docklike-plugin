#include "Group.hpp"

#include "Dock.hpp"
#include "GroupMenu.hpp"

static GtkTargetEntry entries[1] = { { "application/docklike_group", 0 ,0 } };

Group::Group(AppInfo* appInfo, bool pinned):
	mGroupMenu(this)
{
	mButton = gtk_button_new();
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "group");
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "flat");
	
	mAppInfo = appInfo;
	mPinned = pinned;
	mActive = false;

	mTopWindowIndex = 0;

	mSFocus = mSOpened = mSMany = mSHover = false;

	mWindowsCount.setup(0,
		[this]()->uint {
			uint count = 0;
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
		},
		[this](uint windowsCount)->void {
			updateStyle();
		}
	);

	mLeaveTimeout.setup(70, [this](){
		uint distance = mGroupMenu.getPointerDistance();

		if(distance >= mTolerablePointerDistance)
		{
			onMouseLeave();
			return false;
		}

		if(mTolerablePointerDistance > 15)
			mTolerablePointerDistance -= 20;
		else
			mTolerablePointerDistance -= 1;

		return true;
	});

	mMenuShowTimeout.setup(80, [this](){
		onMouseEnter();
		return false;
	});






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

	g_signal_connect(G_OBJECT(mButton), "drag-motion",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, gint x, gint y, guint time, Group* me){
		me->onDragMotion(context, x, y, time);
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "drag-leave",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext *context, guint time, Group* me){
		me->onDragLeave(context, time);
	}), this);

	g_signal_connect(G_OBJECT(mButton), "drag-data-get",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, GtkSelectionData* data, guint info, guint time, Group* me){
		me->onDragDataGet(context, data, info, time);
	}), this);

	g_signal_connect(G_OBJECT(mButton), "drag-data-received",
	G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, gint x, gint y, GtkSelectionData* data, guint info, guint time, Group* me){
		me->onDragDataReceived(context, x, y, data, info, time);
	}), this);

	g_signal_connect(G_OBJECT(mButton), "enter-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventCrossing* event, Group* me){
		me->setStyle(Style::Hover, true);
		me->mLeaveTimeout.stop();
		me->mMenuShowTimeout.start();
		return false;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "leave-notify-event",
	G_CALLBACK(+[](GtkWidget* widget, GdkEventCrossing* event, Group* me){
		me->setStyle(Style::Hover, false);
		me->mMenuShowTimeout.stop();
		if(me->mPinned && me->mWindowsCount == 0)
			me->onMouseLeave();
		else
			me->setMouseLeaveTimeout();
		return true;
	}), this);

	g_signal_connect(G_OBJECT(mButton), "draw",
	G_CALLBACK(+[](GtkWidget* widget, cairo_t* cr, Group* me){
		me->onDraw(cr); return false;
	}), this);

	gtk_drag_source_set(mButton, GDK_BUTTON1_MASK, entries, 1, GDK_ACTION_MOVE);
	gtk_drag_dest_set(mButton, GTK_DEST_DEFAULT_DROP, entries, 1, GDK_ACTION_MOVE);






	if(mPinned) gtk_widget_show(mButton);

	g_object_set_data(G_OBJECT(mButton),"group", this);

	gtk_button_set_relief(GTK_BUTTON(mButton), GTK_RELIEF_NONE);

	gtk_widget_add_events(mButton, GDK_SCROLL_MASK);
	gtk_button_set_always_show_image(GTK_BUTTON(mButton), true);




	if(mAppInfo != NULL && !mAppInfo->icon.empty())
	{
		/*	std::cout << "NEW GROUP:" << mAppInfo->name << std::endl;
			std::cout << "PATH:" << mAppInfo->path << std::endl;
			std::cout << "ICON:" << mAppInfo->icon << std::endl << std::endl;*/
		if(mAppInfo->icon[0] == '/')
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

	resize();
}



void Group::add(GroupWindow* window)
{
	mWindows.push(window);
	mGroupMenu.add(window->mGroupMenuItem);

	mWindowsCount.updateState();
}

void Group::remove(GroupWindow* window)
{
	mWindows.pop(window);
	mGroupMenu.remove(window->mGroupMenuItem);

	mWindowsCount.updateState();
	
	electNewTopWindow(); //TODEL
}




void Group::resize()
{
	gtk_widget_set_size_request(mButton, (((Dock::mPanelSize*1.2)/2)*2)-1, Dock::mPanelSize);

	GtkWidget* img = gtk_button_get_image(GTK_BUTTON(mButton));
	gtk_image_set_pixel_size(GTK_IMAGE(img), Dock::mIconSize);
}

void Group::setStyle(Style style, bool val)
{
	switch(style) {
		case Style::Focus : {
			if(mSFocus != val){ mSFocus = val; gtk_widget_queue_draw(mButton); }
			break;
		}
		case Style::Opened: {
			if(mSOpened != val){ mSOpened = val; gtk_widget_queue_draw(mButton); }
			break;
		}
		case Style::Many: {
			if(mSMany != val){ mSMany = val; gtk_widget_queue_draw(mButton); }
			break;
		}
		case Style::Hover: {
			if(mSHover != val){ mSHover = val; gtk_widget_queue_draw(mButton); }
			break;
		}
	}

}

void Group::onDraw(cairo_t* cr)
{
	double aBack = 0.0;
	
	if(mSHover || mSFocus) aBack = 0.5;
	if(mSHover && mSFocus) aBack = 0.8;

	int w = gtk_widget_get_allocated_width(GTK_WIDGET(mButton));
	int h = gtk_widget_get_allocated_height(GTK_WIDGET(mButton));

	if(aBack > 0){
		cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, aBack);
		cairo_rectangle(cr, 0, 0, w, h);
		cairo_fill(cr);
	}

	if(mSOpened){
		if(mSFocus)
			cairo_set_source_rgba(cr, 0.30, 0.65, 0.90, 1);
		else
			cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 1);
		
		cairo_rectangle(cr, 0, h*0.9231, w, h);
		cairo_fill(cr);
	}

	if(mSMany && (mSOpened || mSHover))
	{
		int x1 = (int) w*0.85;
		cairo_pattern_t *pat = cairo_pattern_create_linear(x1, 0, w, 0);

		cairo_pattern_add_color_stop_rgba(pat, 0.0, 0, 0, 0, 0.45);
		cairo_pattern_add_color_stop_rgba(pat, 0.1, 0, 0, 0, 0.35);
		cairo_pattern_add_color_stop_rgba(pat, 0.3, 0, 0, 0, 0.15);

		if(false && aBack > 0)
			cairo_rectangle(cr, x1, 0, w, h);
		else
			cairo_rectangle(cr, x1, h*0.9231, w, h);

		cairo_set_source(cr, pat);
		cairo_fill(cr);
	}

}

void Group::onMouseEnter()
{
	mLeaveTimeout.stop();

	Dock::mGroups.forEach([](std::pair<AppInfo*, Group*> g)->void
	{ 
		g.second->mGroupMenu.mGroup->onMouseLeave();
	});

	mGroupMenu.popup();

	this->setStyle(Style::Hover, true);
}

void Group::onMouseLeave()
{
	if(!mGroupMenu.mMouseHover)
	{
		this->setStyle(Style::Hover, false);
		mGroupMenu.hide();
	}
}

void Group::setMouseLeaveTimeout()
{
	mTolerablePointerDistance = 215;
	mLeaveTimeout.start();
}




void Group::updateStyle()
{
	int wCount = mWindowsCount;

	if(mPinned || wCount)
		gtk_widget_show(mButton);
	else
		gtk_widget_hide(mButton);

	if(wCount)
		setStyle(Style::Opened, true);
	else
	{
		setStyle(Style::Opened, false);
		setStyle(Style::Focus, false);
	}

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
	mTopWindowIndex = mWindows.getIndex(groupWindow);
}

void Group::onButtonPress(GdkEventButton* event)
{
	std::cout << "PRESS MENU HERE:" << 1 << std::endl;
	
	if(event->button != 3) return;

	if(mWindowsCount == 0)
	{
		GtkWidget* menu = gtk_menu_new();

		GtkWidget* launchAnother = gtk_menu_item_new_with_label("Launch");
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
	}
	else
	{
		GtkWidget* menu = Wnck::getActionMenu(mWindows.get(mTopWindowIndex));

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
}

void Group::onButtonRelease(GdkEventButton* event)
{
	if(event->state & GDK_SHIFT_MASK || (mPinned && mWindowsCount == 0))
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
	if(mPinned && mWindowsCount == 0) return;

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

bool Group::onDragMotion(GdkDragContext* context, int x, int y, guint time)
{
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "drop");

	gdk_drag_status(context, GDK_ACTION_MOVE, time);
	return true;
}

void Group::onDragLeave(const GdkDragContext* context, guint time)
{
	gtk_style_context_remove_class(gtk_widget_get_style_context(mButton), "drop");
}

void Group::onDragDataGet(const GdkDragContext* context, GtkSelectionData* selectionData, guint info, guint time)
{
	Group* me = this;
	std::cout << "pme:" << me << std::endl;

	//TODO is the source object copied or passed by a pointer ?
	gtk_selection_data_set(selectionData, gdk_atom_intern("button", false), 32, (const guchar*)me, sizeof (gpointer)*32);
}

void Group::onDragDataReceived(const GdkDragContext* context, int x, int y, const GtkSelectionData* selectionData, guint info, guint time)
{
	GdkAtom dt = gtk_selection_data_get_data_type(selectionData);
	//if(gdk_atom_name(dt) == "button")
	
	Group* source = (Group*)gtk_selection_data_get_data(selectionData);
	Dock::moveButton(source, this);
}

void Group::onDragBegin(GdkDragContext* context)
{
	gtk_drag_set_icon_name(context, mAppInfo->icon.c_str(), 0, 0);
}