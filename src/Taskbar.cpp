// ** opensource.org/licenses/GPL-3.0

#include "AppInfos.hpp"

#include "Taskbar.hpp"

#define RETURN_IF(b) if(b)return;

namespace Taskbar
{
	GtkWidget* mWidget;
	WnckScreen* mWnckScreen;
	Store::KeyStore<std::string, Group*> mGroups;

	void onWnckWindowOpened(WnckWindow* wnckWindow);
	void onWnckWindowClosed(WnckWindow* wnckWindow);
	void onWnckWindowActivate(WnckWindow* wnckWindow);

	void init()
	{
		mWidget = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_show(mWidget);
		
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


		//already oppened windows
		for (GList* window_l = Wnck::getWindowsList(); window_l != NULL; window_l = window_l->next)
		{
			onWnckWindowOpened(WNCK_WINDOW(window_l->data));
		}

		onWnckWindowActivate(Wnck::getActiveWindow());
	}

	void moveButton(Group* moving, Group* dest)
	{
		int startpos = Help::Gtk::getChildPosition(GTK_CONTAINER(mWidget), GTK_WIDGET(moving->gobj()));
		int destpos = Help::Gtk::getChildPosition(GTK_CONTAINER(mWidget), GTK_WIDGET(dest->gobj()));

		if(startpos == destpos) return;
		if(startpos < destpos) --destpos;
		
		gtk_box_reorder_child(GTK_BOX(mWidget), GTK_WIDGET(moving->gobj()), destpos);
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

			gtk_container_add(GTK_CONTAINER(mWidget), GTK_WIDGET(group->gobj()));
		}
		
		GroupWindow* window = new GroupWindow(wnckWindow, group);
	}

	void onWnckWindowClosed(WnckWindow* wnckWindow)
	{
		std::string groupName = Wnck::getGroupName(wnckWindow);

		std::cout << "closing:" << groupName << std::endl;

		Group* group = mGroups.get(groupName);
		group->removeWindow(Wnck::getXID(wnckWindow));
	}

	void onWnckWindowActivate(WnckWindow* wnckWindow)
	{
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