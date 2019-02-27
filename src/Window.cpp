// ** opensource.org/licenses/GPL-3.0

#include "Window.hpp"

#include <iostream>

nmt::Window::Window(WnckWindow* wnckWindow, Group* group):
	mWnckWindow(wnckWindow),
	mGroup(group)
{ 
	mGroup->addWindow(this);

	init();
}

bool nmt::Window::getState(WnckWindowState flagMask)
{
	return (mState & flagMask) != 0;
}

gulong nmt::Window::getXID()
{
	return wnck_window_get_xid(mWnckWindow);
}

void nmt::Window::activate(guint32 timestamp)
{
	wnck_window_activate(mWnckWindow, timestamp);
}

void nmt::Window::minimize()
{
	wnck_window_minimize(mWnckWindow);
}

void nmt::Window::init()
{
	//initial state
	updateState(wnck_window_get_state(mWnckWindow));

	//signal connection
	g_signal_connect(G_OBJECT(mWnckWindow), "state-changed",
	G_CALLBACK(+[](WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, Window* me)
	{ me->updateState(new_state, changed_mask); }), this);
}

void nmt::Window::updateState(unsigned short state, unsigned short changeMask)
{
	mState = state;

	if(changeMask & WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST)
		mGroup->updateVisibility();
}