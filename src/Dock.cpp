// ** opensource.org/licenses/GPL-3.0

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
		mBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
		gtk_style_context_add_class(gtk_widget_get_style_context(GTK_WIDGET(mBox)), "stld");
		gtk_widget_show(mBox);

		// pinned groups
		std::list<std::string> pinned = Config::getPinned();
		std::list<std::string>::iterator it = pinned.begin();
		while (it != pinned.end())
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
		std::list<std::string> list;

		GList* children = gtk_container_get_children(GTK_CONTAINER(mBox));
		GList* child;
		for (child = children; child; child = child->next)
		{
			GtkWidget* widget = (GtkWidget*)child->data;
			Group* group = (Group*)g_object_get_data(G_OBJECT(widget), "group");

			if (group->mPinned)
			{
				list.push_back(group->mAppInfo->path);
			}
		}

		Config::setPinned(list);
		Config::saveFile();
	}

	void onPanelResize(int size)
	{
		mPanelSize = size;

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

		std::cout << "NEW ICON SIZE:" << mIconSize << std::endl;
		std::cout << "FROM HEIGHT:" << mPanelSize << std::endl;
		std::cout << std::endl;

		mGroups.forEach([](std::pair<AppInfo*, Group*> g) -> void { g.second->resize(); });
	}

	void onPanelOrientationChange(GtkOrientation orientation)
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(mBox), orientation);
	}
} // namespace Dock