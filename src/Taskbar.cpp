// ** opensource.org/licenses/GPL-3.0

#include "Taskbar.hpp"

#define RETURN_IF(b) if(b)return;

nmt::Taskbar::Taskbar():
	mWnckScreen(wnck_screen_get_default())
{
	init();
	show();
}

nmt::Taskbar::~Taskbar()
{

}

void nmt::Taskbar::init()
{
	wnck_screen_force_update (mWnckScreen);

	//already oppened windows
	for (GList* window_l = wnck_screen_get_windows (mWnckScreen); window_l != NULL; window_l = window_l->next)
    {
      onWnckWindowOpened(WNCK_WINDOW(window_l->data));
    }

	onWnckWindowActivate(wnck_screen_get_active_window(mWnckScreen));

	//signal connection
	g_signal_connect(G_OBJECT(mWnckScreen), "window-opened",
	G_CALLBACK(+[](WnckScreen* screen, WnckWindow* window, Taskbar* me)
	{
		me->onWnckWindowOpened(window);
	}), this);

	g_signal_connect(G_OBJECT(mWnckScreen), "window-closed",
	G_CALLBACK(+[](WnckScreen* screen, WnckWindow* window, Taskbar* me)
	{
		me->onWnckWindowClosed(window);
	}), this);

	g_signal_connect(G_OBJECT(mWnckScreen), "active-window-changed",
	G_CALLBACK(+[](WnckScreen* screen, WnckWindow* previously_active_window, Taskbar* me)
	{ 
		WnckWindow* window = wnck_screen_get_active_window(screen);
		RETURN_IF(!WNCK_IS_WINDOW (window));
		me->onWnckWindowActivate(window);
	}), this);
}

void nmt::Taskbar::onWnckWindowOpened(WnckWindow* wnckWindow)
{
	std::string classGroupName = wnck_window_get_class_group_name(wnckWindow);

	Group* group = mGroups.get(classGroupName);
	if(group == NULL)
	{
		group = new Group(classGroupName);
		mGroups.push(classGroupName, group);

		add(*group);
	}
	
	Window* window = new Window(wnckWindow, group);
}

void nmt::Taskbar::onWnckWindowClosed(WnckWindow* wnckWindow)
{
	std::string classGroupName = wnck_window_get_class_group_name(wnckWindow);

	Group* group = mGroups.get(classGroupName);
	group->removeWindow(wnck_window_get_xid(wnckWindow));
}

void nmt::Taskbar::onWnckWindowActivate(WnckWindow* wnckWindow)
{
	gulong XID = wnck_window_get_xid(wnckWindow);
	std::string classGroupName = wnck_window_get_class_group_name(wnckWindow);
	
	mGroups.forEach([&classGroupName, &XID](std::pair<std::string, Group*> g)->void
	{
		if(g.first == classGroupName)
			g.second->onWindowActivate(XID);
		else
			g.second->onWindowUnactivate();
	});
	//Group* group = mGroups.get(classGroupName);
}