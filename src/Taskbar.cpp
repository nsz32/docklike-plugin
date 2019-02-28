// ** opensource.org/licenses/GPL-3.0

#include "Taskbar.hpp"

#define RETURN_IF(b) if(b)return;

nmt::Taskbar::Taskbar()
{
	mWnckScreen = nmt::Utils::Wnck::getScreen();

	init();
	show();
}

nmt::Taskbar::~Taskbar()
{

}

void nmt::Taskbar::init()
{
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
		WnckWindow* window = nmt::Utils::Wnck::getActiveWindow();
		RETURN_IF(!WNCK_IS_WINDOW (window));
		me->onWnckWindowActivate(window);
	}), this);


	//already oppened windows
	for (GList* window_l = nmt::Utils::Wnck::getWindowsList(); window_l != NULL; window_l = window_l->next)
	{
		onWnckWindowOpened(WNCK_WINDOW(window_l->data));
	}

	onWnckWindowActivate(nmt::Utils::Wnck::getActiveWindow());
}

void nmt::Taskbar::onWnckWindowOpened(WnckWindow* wnckWindow)
{
	std::string groupName = nmt::Utils::Wnck::getGroupName(wnckWindow);

	Group* group = mGroups.get(groupName);
	if(group == NULL)
	{
		group = new Group(groupName);
		mGroups.push(groupName, group);

		add(*group);
	}
	
	Window* window = new Window(wnckWindow, group);
}

void nmt::Taskbar::onWnckWindowClosed(WnckWindow* wnckWindow)
{
	std::string groupName = nmt::Utils::Wnck::getGroupName(wnckWindow);

	Group* group = mGroups.get(groupName);
	group->removeWindow(nmt::Utils::Wnck::getXID(wnckWindow));
}

void nmt::Taskbar::onWnckWindowActivate(WnckWindow* wnckWindow)
{
	gulong XID = nmt::Utils::Wnck::getXID(wnckWindow);
	std::string groupName = nmt::Utils::Wnck::getGroupName(wnckWindow);
	
	mGroups.forEach([&groupName, &XID](std::pair<std::string, Group*> g)->void
	{
		if(g.first == groupName)
			g.second->onWindowActivate(XID);
		else
			g.second->onWindowUnactivate();
	});
}