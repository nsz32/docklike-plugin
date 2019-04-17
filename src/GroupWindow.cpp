// ** opensource.org/licenses/GPL-3.0

#include "GroupWindow.hpp"

GroupWindow::GroupWindow(WnckWindow* wnckWindow, Group* group):
	mWnckWindow(wnckWindow),
	mDockButtonMenuItem(this),
	mGroup(group)
{ 
	mDockButtonMenuItem.setLabel(wnck_window_get_name(wnckWindow));
	mGroup->addWindow(this);
	

	//initial state
	updateState(Wnck::getState(mWnckWindow));

	//signal connection
	g_signal_connect(G_OBJECT(mWnckWindow), "name-changed",
	G_CALLBACK(+[](WnckWindow* window, GroupWindow* me)
	{ 
		me->mDockButtonMenuItem.setLabel(wnck_window_get_name(window));
	}), this);

	g_signal_connect(G_OBJECT(mWnckWindow), "state-changed",
	G_CALLBACK(+[](WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, GroupWindow* me)
	{ 
		me->updateState(new_state, changed_mask);
	}), this);
}

bool GroupWindow::getState(WnckWindowState flagMask)
{
	return (mState & flagMask) != 0;
}

gulong GroupWindow::getXID()
{
	return Wnck::getXID(mWnckWindow);
}

void GroupWindow::activate(guint32 timestamp)
{
	Wnck::activate(mWnckWindow, timestamp);
}

void GroupWindow::minimize()
{
	Wnck::minimize(mWnckWindow);
}

void GroupWindow::showMenu()
{
	
}

void GroupWindow::updateState(ushort state, ushort changeMask)
{
	mState = state;

	if(changeMask & WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST)
		mGroup->updateStyle();
}