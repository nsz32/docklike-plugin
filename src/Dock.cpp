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
		gtk_widget_show(mBox);

		//pinned groups
		std::list<std::string> pinned = Plugin::mConfig->getPinned();
		std::list<std::string>::iterator it = pinned.begin();
		while(it != pinned.end())
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
		if(group == NULL)
		{
			std::cout << "NEW GROUP:" << appInfo->name << std::endl;
			group = new Group(appInfo, false);
			mGroups.push(appInfo, group);

			gtk_container_add(GTK_CONTAINER(mBox), GTK_WIDGET(group->mButton));
		}

		return group;
	}

	void moveButton(DockButton* moving, DockButton* dest)
	{
		int startpos = Help::Gtk::getChildPosition(GTK_CONTAINER(mBox), GTK_WIDGET(moving->mButton));
		int destpos = Help::Gtk::getChildPosition(GTK_CONTAINER(mBox), GTK_WIDGET(dest->mButton));

		if(startpos == destpos) return;
		if(startpos < destpos) --destpos;
		
		gtk_box_reorder_child(GTK_BOX(mBox), GTK_WIDGET(moving->mButton), destpos);

		savePinned();
	}

	void savePinned()
	{
		std::list<std::string> list;

		GList* children = gtk_container_get_children(GTK_CONTAINER(mBox));
		GList* child;
		for(child = children; child; child = child->next)
		{
			GtkWidget* widget = (GtkWidget*)child->data;
			Group* group = (Group*)g_object_get_data(G_OBJECT(widget), "group");

			if(group->mPinned)
			{
				list.push_back(group->mAppInfo->path);
			}
		}
		
		Plugin::mConfig->setPinned(list);
		Plugin::mConfig->save();
	}
	
	void onPanelResize(int size)
	{

		mPanelSize = size;

		#if LIBXFCE4PANEL_CHECK_VERSION(4,13,0)
			mIconSize = xfce_panel_plugin_get_icon_size(XFCE_PANEL_PLUGIN(Plugin::mXfPlugin));
		#else
			GtkStyleContext* context = gtk_widget_get_style_context(GTK_WIDGET(mGroups.first()->mButton));
			GtkBorder padding, border;
			gtk_style_context_get_padding (context, gtk_widget_get_state_flags(GTK_WIDGET(mBox)), &padding);
			gtk_style_context_get_border (context, gtk_widget_get_state_flags(GTK_WIDGET(mBox)), &border);
			int xthickness = padding.left + padding.right + border.left + border.right;
			int ythickness = padding.top + padding.bottom + border.top + border.bottom;
			
			int width = Dock::mPanelSize - MAX(xthickness, ythickness);
				
			if (width <= 21)
				mIconSize = 16;
			else if (width >=22 && width <= 29)
				mIconSize = 24;
			else if (width >= 30 && width <= 40)
				mIconSize = 32;
			else
				mIconSize = width;
		#endif

		mGroups.forEach([](std::pair<AppInfo*, Group*> g)->void { g.second->resize(); });
	}
}