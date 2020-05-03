/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Group.hpp"

#include "Dock.hpp"
#include "GroupMenu.hpp"

static GtkTargetEntry entries[1] = {{"application/docklike_group", 0, 0}};
static GtkTargetList* targetList = gtk_target_list_new(entries, 1);

Group::Group(AppInfo* appInfo, bool pinned) : mGroupMenu(this)
{
	mButton = gtk_button_new();
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "group");
	gtk_style_context_add_class(gtk_widget_get_style_context(mButton), "flat");

	mIconPixbuf = NULL;

	mAppInfo = appInfo;
	mPinned = pinned;
	mActive = false;

	mTopWindowIndex = 0;

	mSFocus = mSOpened = mSMany = mSHover = mSSuper = false;

	mWindowsCount.setup(
		0, [this]() -> uint {
			uint count = 0;
			mWindows.findIf([&count](GroupWindow* e) -> bool {
				if (!e->getState(WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST))
				{
				++count;
				if (count == 2) return true;
				}
			return false;
		});

		return count; },
		[this](uint windowsCount) -> void { updateStyle(); });

	mLeaveTimeout.setup(40, [this]() {
		uint distance = mGroupMenu.getPointerDistance();

		if (distance >= mTolerablePointerDistance)
		{
			onMouseLeave();
			return false;
		}

		mTolerablePointerDistance -= 10;

		return true;
	});

	mMenuShowTimeout.setup(90, [this]() {
		onMouseEnter();
		return false;
	});

	g_signal_connect(
		G_OBJECT(mButton), "button-press-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEventButton* event, Group* me) {
			gdk_device_ungrab((event)->device, (event)->time);
			if (event->button != 3 && event->state & GDK_CONTROL_MASK)
			{
				std::cout << "STARTDRAG:state:" << event->state << std::endl;
				gtk_drag_begin_with_coordinates(widget, targetList, GDK_ACTION_MOVE, event->button, (GdkEvent*)event, -1, -1);
			}
			if (event->state & GDK_CONTROL_MASK)
			{
				me->mGroupMenu.hide();
				return false;
			}

			me->onButtonPress(event);
			return true;
		}),
		this);

	g_signal_connect(
		G_OBJECT(mButton), "button-release-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEventButton* event, Group* me) {
			if (event->button != 1)
				return false;
			me->onButtonRelease(event);
			return true;
		}),
		this);

	g_signal_connect(
		G_OBJECT(mButton), "scroll-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEventScroll* event, Group* me) {
			me->scrollWindows(event->time, event->direction);
			return true;
		}),
		this);

	g_signal_connect(G_OBJECT(mButton), "drag-begin",
		G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, Group* me) {
			me->onDragBegin(context);
		}),
		this);

	g_signal_connect(G_OBJECT(mButton), "drag-motion",
		G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, gint x, gint y, guint time, Group* me) {
			return me->onDragMotion(widget, context, x, y, time);
		}),
		this);

	g_signal_connect(G_OBJECT(mButton), "drag-leave",
		G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, guint time, Group* me) {
			me->onDragLeave(context, time);
		}),
		this);

	g_signal_connect(G_OBJECT(mButton), "drag-data-get",
		G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, GtkSelectionData* data, guint info, guint time, Group* me) {
			me->onDragDataGet(context, data, info, time);
		}),
		this);

	g_signal_connect(G_OBJECT(mButton), "drag-data-received",
		G_CALLBACK(+[](GtkWidget* widget, GdkDragContext* context, gint x, gint y, GtkSelectionData* data, guint info, guint time, Group* me) {
			me->onDragDataReceived(context, x, y, data, info, time);
		}),
		this);

	g_signal_connect(G_OBJECT(mButton), "enter-notify-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEventCrossing* event, Group* me) {
			if (!me->mActive && event->state & (GDK_BUTTON1_MASK))
			{
				me->mActiveBeforePressed = false;
				me->activate(event->time);
			}
			else if (me->mActive)
			{
				me->mActiveBeforePressed = true;
			}

			me->setStyle(Style::Hover, true);
			me->mLeaveTimeout.stop();
			me->mMenuShowTimeout.start();
			return false;
		}),
		this);

	g_signal_connect(
		G_OBJECT(mButton), "leave-notify-event",
		G_CALLBACK(+[](GtkWidget* widget, GdkEventCrossing* event, Group* me) {
			me->setStyle(Style::Hover, false);
			me->mMenuShowTimeout.stop();
			if (me->mPinned && me->mWindowsCount == 0)
				me->onMouseLeave();
			else
				me->setMouseLeaveTimeout();
			return true;
		}),
		this);

	g_signal_connect(
		G_OBJECT(mButton), "draw", G_CALLBACK(+[](GtkWidget* widget, cairo_t* cr, Group* me) {
			me->onDraw(cr);
			return false;
		}),
		this);

	gtk_drag_dest_set(mButton, GTK_DEST_DEFAULT_DROP, entries, 1, GDK_ACTION_MOVE);

	if (mPinned)
		gtk_widget_show(mButton);

	g_object_set_data(G_OBJECT(mButton), "group", this);

	gtk_button_set_relief(GTK_BUTTON(mButton), GTK_RELIEF_NONE);

	gtk_widget_add_events(mButton, GDK_SCROLL_MASK);
	gtk_button_set_always_show_image(GTK_BUTTON(mButton), true);

	if (mAppInfo != NULL && !mAppInfo->icon.empty())
	{
		GtkWidget* icon;

		if (mAppInfo->icon[0] == '/')
			mIconPixbuf = gdk_pixbuf_new_from_file(mAppInfo->icon.c_str(), NULL);
		else
		{
			icon = gtk_image_new_from_icon_name(mAppInfo->icon.c_str(), GTK_ICON_SIZE_BUTTON);
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
	mWindowsCount.updateState();

	mGroupMenu.add(window->mGroupMenuItem);

	if (mWindowsCount == 1 && !mPinned)
	{
		std::cout << "REORDER OK:" << 0 << std::endl;
		gtk_box_reorder_child(GTK_BOX(Dock::mBox), GTK_WIDGET(mButton), -1);
	}
}

void Group::remove(GroupWindow* window)
{
	mWindows.pop(window);
	mWindowsCount.updateState();

	mGroupMenu.remove(window->mGroupMenuItem);

	electNewTopWindow(); // TODEL

	setStyle(Style::Focus, false);
}

void Group::activate(guint32 timestamp)
{
	if (mWindowsCount == 0)
		return;

	GroupWindow* groupWindow = mWindows.get(mTopWindowIndex);

	mWindows.forEach([&timestamp, &groupWindow](GroupWindow* w) -> void {
		if (w != groupWindow)
			w->activate(timestamp);
	});

	groupWindow->activate(timestamp);
}

void Group::scrollWindows(guint32 timestamp, GdkScrollDirection direction)
{
	if (mPinned && mWindowsCount == 0)
		return;

	if (!mActive)
	{
		mWindows.get(mTopWindowIndex)->activate(timestamp);
	}
	else
	{
		if (direction == GDK_SCROLL_UP)
			mTopWindowIndex = ++mTopWindowIndex % mWindows.size();
		else if (direction == GDK_SCROLL_DOWN)
		{
			int size = mWindows.size();
			mTopWindowIndex = (--mTopWindowIndex + size) % size;
		}
		mWindows.get(mTopWindowIndex)->activate(timestamp);
	}
}

void Group::resize()
{
	gtk_widget_set_size_request(mButton, (round((Dock::mPanelSize * 1.2) / 2) * 2), Dock::mPanelSize);

	GtkWidget* img;

	if (mIconPixbuf != NULL)
	{
		GdkPixbuf* pixbuf = gdk_pixbuf_scale_simple(mIconPixbuf, Dock::mIconSize, Dock::mIconSize, GDK_INTERP_HYPER);
		GtkWidget* icon = gtk_image_new_from_pixbuf(pixbuf);
		gtk_button_set_image(GTK_BUTTON(mButton), icon);
		img = gtk_button_get_image(GTK_BUTTON(mButton));
	}
	else
	{
		img = gtk_button_get_image(GTK_BUTTON(mButton));
		gtk_image_set_pixel_size(GTK_IMAGE(img), Dock::mIconSize);
	}

	gtk_widget_set_valign(img, GTK_ALIGN_CENTER);
}

void Group::setStyle(Style style, bool val)
{
	switch (style)
	{
	case Style::Focus:
	{
		if (mSFocus != val)
		{
			mSFocus = val;
			gtk_widget_queue_draw(mButton);
		}
		break;
	}
	case Style::Opened:
	{
		if (mSOpened != val)
		{
			mSOpened = val;
			gtk_widget_queue_draw(mButton);
		}
		break;
	}
	case Style::Many:
	{
		if (mSMany != val)
		{
			mSMany = val;
			gtk_widget_queue_draw(mButton);
		}
		break;
	}
	case Style::Hover:
	{
		if (mSHover != val)
		{
			mSHover = val;
			gtk_widget_queue_draw(mButton);
		}
		break;
	}
	case Style::Super:
	{
		if (mSSuper != val)
		{
			mSSuper = val;
			gtk_widget_queue_draw(mButton);
		}
		break;
	}
	}
}

void Group::onDraw(cairo_t* cr)
{
	int w = gtk_widget_get_allocated_width(GTK_WIDGET(mButton));
	int h = gtk_widget_get_allocated_height(GTK_WIDGET(mButton));

	double aBack = 0;

	if (mSSuper)
		aBack += 0.25;
	if (mSHover && mSFocus)
		aBack = 0.8;
	else if (mSHover || mSFocus)
		aBack += 0.5;

	if (aBack > 0)
	{
		cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, aBack);
		cairo_rectangle(cr, 0, 0, w, h);
		cairo_fill(cr);
	}

	if (Settings::indicatorStyle == 0) // Bar
	{
		if (mSOpened)
		{
			if (mSFocus)
				cairo_set_source_rgba(cr, 0.30, 0.65, 0.90, 1);
			else
				cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 1);

			cairo_rectangle(cr, 0, round(h * 0.9231), w, h);
			cairo_fill(cr);
		}

		if (mSMany && (mSOpened || mSHover))
		{
			int x1 = (int)w * 0.88;
			cairo_pattern_t* pat = cairo_pattern_create_linear(x1, 0, w, 0);

			cairo_pattern_add_color_stop_rgba(pat, 0.0, 0, 0, 0, 0.45);
			cairo_pattern_add_color_stop_rgba(pat, 0.1, 0, 0, 0, 0.35);
			cairo_pattern_add_color_stop_rgba(pat, 0.3, 0, 0, 0, 0.15);

			if (aBack > 0)
				cairo_rectangle(cr, x1, 0, w, h);
			else
				cairo_rectangle(cr, x1, round(h * 0.9231), w, h);

			cairo_set_source(cr, pat);
			cairo_fill(cr);

			cairo_pattern_destroy(pat);
		}
	}
	else if (Settings::indicatorStyle == 1) // Dots
	{
		if (mSOpened)
		{
			double dotRadius = std::max(h * (0.093), 2.);
			double ypos = h * 0.99;

			double rgb[3] = {0, 1, 2};

			if (mSFocus)
			{
				rgb[0] = 0.30;
				rgb[1] = 0.65;
				rgb[2] = 0.90;
			}
			else
			{
				rgb[0] = 0.7;
				rgb[1] = 0.7;
				rgb[2] = 0.7;
			}

			if (mSMany)
			{
				double cx = (w / 2.) - dotRadius * 1;

				cairo_pattern_t* pat = cairo_pattern_create_radial(cx, ypos, 0, cx, ypos, dotRadius);
				cairo_pattern_add_color_stop_rgba(pat, 0.3, rgb[0], rgb[1], rgb[2], 1);
				cairo_pattern_add_color_stop_rgba(pat, 1, rgb[0], rgb[1], rgb[2], 0.15);
				cairo_set_source(cr, pat);

				cairo_arc(cr, cx, ypos, dotRadius, 0.0, 2.0 * M_PI);
				cairo_fill(cr);

				cairo_pattern_destroy(pat);

				cx = (w / 2.) + dotRadius * 1;

				pat = cairo_pattern_create_radial(cx, ypos, 0, cx, ypos, dotRadius);
				cairo_pattern_add_color_stop_rgba(pat, 0.3, rgb[0], rgb[1], rgb[2], 1);
				cairo_pattern_add_color_stop_rgba(pat, 1, rgb[0], rgb[1], rgb[2], 0.15);
				cairo_set_source(cr, pat);

				cairo_arc(cr, cx, ypos, dotRadius, 0.0, 2.0 * M_PI);
				cairo_fill(cr);

				cairo_pattern_destroy(pat);
			}
			else
			{
				double cx = w / 2.;

				cairo_pattern_t* pat = cairo_pattern_create_radial(cx, ypos, 0, cx, ypos, dotRadius);
				cairo_pattern_add_color_stop_rgba(pat, 0.3, rgb[0], rgb[1], rgb[2], 1);
				cairo_pattern_add_color_stop_rgba(pat, 1, rgb[0], rgb[1], rgb[2], 0.15);
				cairo_set_source(cr, pat);

				cairo_arc(cr, cx, ypos, dotRadius, 0.0, 2.0 * M_PI);
				cairo_fill(cr);

				cairo_pattern_destroy(pat);
			}
		}
	}
}

void Group::onMouseEnter()
{
	mLeaveTimeout.stop();

	Dock::mGroups.forEach([this](std::pair<AppInfo*, Group*> g) -> void {
		if (&(g.second->mGroupMenu) != &(this->mGroupMenu))
			g.second->mGroupMenu.mGroup->onMouseLeave();
	});

	mGroupMenu.popup();

	this->setStyle(Style::Hover, true);
}

void Group::onMouseLeave()
{
	if (!mGroupMenu.mMouseHover)
	{
		this->setStyle(Style::Hover, false);
		mGroupMenu.hide();
	}
}

void Group::setMouseLeaveTimeout()
{
	mTolerablePointerDistance = 200;
	mLeaveTimeout.start();
}

void Group::updateStyle()
{
	int wCount = mWindowsCount;

	if (mPinned || wCount)
		gtk_widget_show(mButton);
	else
		gtk_widget_hide(mButton);

	if (wCount)
		setStyle(Style::Opened, true);
	else
	{
		setStyle(Style::Opened, false);
		setStyle(Style::Focus, false);
	}

	if (wCount > 1)
		setStyle(Style::Many, true);
	else
		setStyle(Style::Many, false);
}

void Group::electNewTopWindow()
{
	if (mWindows.size() > 0)
	{
		GroupWindow* newTopWindow;

		if (mWindows.size() == 1)
			newTopWindow = mWindows.get(0);
		else
		{
			newTopWindow = Wnck::mGroupWindows.findIf([this](std::pair<gulong, GroupWindow*> e) -> bool {
				if (e.second->mGroup == this)
					return true;
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
	setStyle(Style::Focus, false);
	mActive = false;
}

void Group::setTopWindow(GroupWindow* groupWindow) { mTopWindowIndex = mWindows.getIndex(groupWindow); }

void Group::onButtonPress(GdkEventButton* event)
{
	if (event->button == 3)
	{
		GtkWidget* menu = Wnck::buildActionMenu(mWindows.get(mTopWindowIndex), this);

		gtk_menu_attach_to_widget(GTK_MENU(menu), GTK_WIDGET(mButton), NULL);
		gtk_menu_popup_at_widget(GTK_MENU(menu), GTK_WIDGET(mButton), GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, (GdkEvent*)event);
	}
}

void Group::onButtonRelease(GdkEventButton* event)
{
	if (event->state & GDK_SHIFT_MASK || (mPinned && mWindowsCount == 0))
	{
		AppInfos::launch(mAppInfo);
	}
	else if (mActive && mActiveBeforePressed)
	{
		mWindows.get(mTopWindowIndex)->minimize();
	}
}

bool Group::onDragMotion(GtkWidget* widget, GdkDragContext* context, int x, int y, guint time)
{
	GdkModifierType mask;

	gdk_window_get_pointer(gtk_widget_get_window(widget), NULL, NULL, &mask);
	if (mask & GDK_CONTROL_MASK)
		gtk_drag_cancel(context);

	GList* tmp_list = gdk_drag_context_list_targets(context);
	if (tmp_list != NULL)
	{
		char* name = gdk_atom_name(GDK_POINTER_TO_ATOM(tmp_list->data));
		std::string target = name;
		g_free(name);

		if (target != "application/docklike_group")
		{
			if (mWindowsCount > 0)
			{
				GroupWindow* groupWindow = mWindows.get(mTopWindowIndex);

				groupWindow->activate(time);

				if (!mGroupMenu.mVisible)
					onMouseEnter();
			}

			gdk_drag_status(context, GDK_ACTION_DEFAULT, time);
			return true;
		}
	}

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

	// TODO is the source object copied or passed by a pointer ?
	gtk_selection_data_set(selectionData, gdk_atom_intern("button", false), 32, (const guchar*)me, sizeof(gpointer) * 32);
}

void Group::onDragDataReceived(const GdkDragContext* context, int x, int y, const GtkSelectionData* selectionData,
	guint info, guint time)
{
	GdkAtom dt = gtk_selection_data_get_data_type(selectionData);
	// if(gdk_atom_name(dt) == "button")

	Group* source = (Group*)gtk_selection_data_get_data(selectionData);
	Dock::moveButton(source, this);
}

void Group::onDragBegin(GdkDragContext* context) { gtk_drag_set_icon_name(context, mAppInfo->icon.c_str(), 0, 0); }