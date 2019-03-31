// ** opensource.org/licenses/GPL-3.0

#include "Dock.hpp"

#define RETURN_IF(b) if(b)return;

namespace Dock
{
	GtkWidget* mBox;
	WnckScreen* mWnckScreen;
	Store::KeyStore<std::string, Group*> mGroups;

	int mPanelSize;
	int mIconSize;

	void onWnckWindowOpened(WnckWindow* wnckWindow);
	void onWnckWindowClosed(WnckWindow* wnckWindow);
	void onWnckWindowActivate(WnckWindow* wnckWindow);

	void init(XfcePanelPlugin* xfPlugin)
	{
		mBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
		gtk_widget_show(mBox);

		mWnckScreen = Wnck::getScreen();

		//signal connection
		g_signal_connect(G_OBJECT(mWnckScreen), "window-opened",
		G_CALLBACK(+[](WnckScreen* screen, WnckWindow* window)
		{
			onWnckWindowOpened(window);
		}), NULL);

		g_signal_connect(G_OBJECT(mWnckScreen), "window-closed",
		G_CALLBACK(+[](WnckScreen* screen, WnckWindow* window)
		{
			onWnckWindowClosed(window);
		}), NULL);

		g_signal_connect(G_OBJECT(mWnckScreen), "active-window-changed",
		G_CALLBACK(+[](WnckScreen* screen, WnckWindow* previously_active_window)
		{ 
			WnckWindow* window = Wnck::getActiveWindow();
			RETURN_IF(!WNCK_IS_WINDOW (window));
			onWnckWindowActivate(window);
		}), NULL);



		

		//pinned windows
		std::list<std::string> pinned = Plugin::mConfig->getPinned();
		std::list<std::string>::iterator it = pinned.begin();
		while(it != pinned.end())
		{
			std::string groupName = *it;
			if(++it == pinned.end()) break;
			AppInfo* appInfo = AppInfos::search(*it);
			++it;

			std::cout << "PINNED?:" << appInfo->path << std::endl;

			Group* group = new Group(groupName, appInfo, true);
			mGroups.push(groupName, group);

			gtk_container_add(GTK_CONTAINER(mBox), GTK_WIDGET(group->mButton));
		}

		//already oppened windows
		for (GList* window_l = Wnck::getWindowsList(); window_l != NULL; window_l = window_l->next)
		{
			onWnckWindowOpened(WNCK_WINDOW(window_l->data));
		}

		onWnckWindowActivate(Wnck::getActiveWindow());
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
				list.push_back(group->mGroupName);
				list.push_back(group->mAppInfo->path);
			}
		}
		
		Plugin::mConfig->setPinned(list);
		Plugin::mConfig->save();
	}
	
	void onPanelResize(int size)
	{
		mPanelSize = size;

		#if LIBXFCE4PANEL_CHECK_VERSION (4,13,0)
			mIconSize = xfce_panel_plugin_get_icon_size(XFCE_PANEL_PLUGIN(Plugin::mXfPlugin));
		#else
			GtkStyleContext* context = gtk_widget_get_style_context(GTK_WIDGET(mGroups.first()->mButton));
			GtkBorder padding, border;
			gtk_style_context_get_padding (context, gtk_widget_get_state_flags(GTK_WIDGET(mBox)), &padding);
			gtk_style_context_get_border (context, gtk_widget_get_state_flags(GTK_WIDGET(mBox)), &border);
			int xthickness = padding.left + padding.right + border.left + border.right;
			int ythickness = padding.top + padding.bottom + border.top + border.bottom;

			std::cout << "BORDER:" << MAX(xthickness, ythickness) << std::endl;

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

		mGroups.forEach([](std::pair<std::string, Group*> g)->void { g.second->resize(); });
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



	void onWnckWindowOpened(WnckWindow* wnckWindow)
	{
		std::string groupName = Wnck::getGroupName(wnckWindow);
		AppInfo* appInfo = AppInfos::search(groupName);

		Group* group = mGroups.get(groupName);
		if(group == NULL)
		{
			group = new Group(groupName, appInfo, false);
			mGroups.push(groupName, group);

			gtk_container_add(GTK_CONTAINER(mBox), GTK_WIDGET(group->mButton));
		}
		
		GroupWindow* window = new GroupWindow(wnckWindow, group);
	}

	void onWnckWindowClosed(WnckWindow* wnckWindow)
	{
		std::string groupName = Wnck::getGroupName(wnckWindow);

		std::cout << "closing:" << groupName << std::endl;

		Group* group = mGroups.get(groupName);
		group->removeWindow(Wnck::getXID(wnckWindow));

		if(!group->hasVisibleWindows() && !group->mPinned)
			gtk_box_reorder_child(GTK_BOX(mBox), GTK_WIDGET(group->mButton), -1);
	}

	void onWnckWindowActivate(WnckWindow* wnckWindow)
	{
		RETURN_IF(wnckWindow == NULL);

		gulong XID = Wnck::getXID(wnckWindow);
		std::string groupName = Wnck::getGroupName(wnckWindow);
		
		mGroups.forEach([&groupName, &XID](std::pair<std::string, Group*> g)->void
		{
			if(g.first == groupName)
				g.second->onWindowActivate(XID);
			else
				g.second->onWindowUnactivate();
		});
	}

}