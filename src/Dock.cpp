/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Dock.hpp"

namespace Dock
{
	GtkWidget* mBox;
	Store::KeyStore<AppInfo*, Group*> mGroups;

	int mPanelSize;
	int mIconSize;

	void onWnckWindowOpened(WnckWindow* wnckWindow);
	void onWnckWindowClosed(WnckWindow* wnckWindow);
	void onWnckWindowActivate(WnckWindow* wnckWindow);

	void init()
	{
		mBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(mBox)), "stld");
		gtk_widget_show(mBox);

		// pinned groups
		std::list<std::string> pinnedApps = Settings::pinnedAppList;
		std::list<std::string>::iterator it = pinnedApps.begin();
		while (it != pinnedApps.end())
		{
			AppInfo* appInfo = AppInfos::search(*it);

			Group* group = new Group(appInfo, true);
			mGroups.push(appInfo, group);

			gtk_container_add(GTK_CONTAINER(mBox), GTK_WIDGET(group->mButton));

			++it;
		}
	}

	Group* prepareGroup(AppInfo* appInfo)
	{
		Group* group = mGroups.get(appInfo);
		if (group == NULL)
		{
			group = new Group(appInfo, false);
			mGroups.push(appInfo, group);

			gtk_container_add(GTK_CONTAINER(mBox), GTK_WIDGET(group->mButton));
		}

		return group;
	}

	void moveButton(Group* moving, Group* dest)
	{
		int startpos = Help::Gtk::getChildPosition(GTK_CONTAINER(mBox), GTK_WIDGET(moving->mButton));
		int destpos = Help::Gtk::getChildPosition(GTK_CONTAINER(mBox), GTK_WIDGET(dest->mButton));

		if (startpos == destpos)
			return;
		if (startpos < destpos)
			--destpos;

		gtk_box_reorder_child(GTK_BOX(mBox), GTK_WIDGET(moving->mButton), destpos);

		savePinned();
	}

	void savePinned()
	{
		std::list<std::string> pinnedList;

		GList* children = gtk_container_get_children(GTK_CONTAINER(mBox));
		GList* child;
		for (child = children; child; child = child->next)
		{
			GtkWidget* widget = (GtkWidget*)child->data;
			Group* group = (Group*)g_object_get_data(G_OBJECT(widget), "group");

			if (group->mPinned)
			{
				pinnedList.push_back(group->mAppInfo->path);
			}
		}

		Settings::pinnedAppList.set(pinnedList);
	}

	void redraw()
	{
		gtk_widget_queue_draw(mBox);
	}

	void hoverSupered(bool on)
	{
		int grabbedKeys = Hotkeys::mGrabbedKeys;
		GList* children = gtk_container_get_children(GTK_CONTAINER(mBox));
		for (GList* child = children; child && grabbedKeys; child = child->next)
		{
			GtkWidget* widget = (GtkWidget*)child->data;
			if (!gtk_widget_get_visible(widget))
				continue;

			Group* group = (Group*)g_object_get_data(G_OBJECT(widget), "group");
			group->setStyle(Group::Style::Super, on);
			--grabbedKeys;
		}
	}

	void activateGroup(int nb, guint32 timestamp)
	{
		int i = 0;
		GList* children = gtk_container_get_children(GTK_CONTAINER(mBox));
		for (GList* child = children; child; child = child->next)
		{
			GtkWidget* widget = (GtkWidget*)child->data;
			if (gtk_widget_get_visible(widget))
				if (i == nb)
				{
					Group* group = (Group*)g_object_get_data(G_OBJECT(widget), "group");
					if (group->mSFocus)
						group->scrollWindows(timestamp, GDK_SCROLL_DOWN);
					else if (group->mWindowsCount > 0)
						group->activate(timestamp);
					else
						AppInfos::launch(group->mAppInfo);
					return;
				}
				else
					++i;
		}
	}

	void onPanelResize(int size)
	{
		if (size != -1)
			mPanelSize = size;

		gtk_box_set_spacing(GTK_BOX(mBox), mPanelSize / 10);

		if (Settings::forceIconSize)
		{
			mIconSize = Settings::iconSize;
		}
		else
		{
			if (mPanelSize <= 20)
				mIconSize = mPanelSize - 6;
			else if (mPanelSize <= 28)
				mIconSize = 16;
			else if (mPanelSize <= 38)
				mIconSize = 24;
			else if (mPanelSize <= 41)
				mIconSize = 32;
			else
				mIconSize = mPanelSize * 0.8;
		}

		std::cout << "mPanelSize:" << mPanelSize << std::endl;
		std::cout << "mIconSize:" << mIconSize << std::endl;

		mGroups.forEach([](std::pair<AppInfo*, Group*> g) -> void { g.second->resize(); });
	}

	void onPanelOrientationChange(GtkOrientation orientation)
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(mBox), orientation);
	}
} // namespace Dock