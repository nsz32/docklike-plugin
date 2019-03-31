// ** opensource.org/licenses/GPL-3.0

#include "GroupWindow.hpp"

GroupWindow::GroupWindow(WnckWindow* wnckWindow, Group* group):
	mWnckWindow(wnckWindow),
	mGroup(group)
{ 
	mGroup->addWindow(this);

	init();
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

void GroupWindow::init()
{
	//initial state
	updateState(Wnck::getState(mWnckWindow));

	//signal connection
	g_signal_connect(G_OBJECT(mWnckWindow), "state-changed",
	G_CALLBACK(+[](WnckWindow* window, WnckWindowState changed_mask, WnckWindowState new_state, GroupWindow* me)
	{ 
		me->updateState(new_state, changed_mask);
		}), this);
}

void GroupWindow::updateState(ushort state, ushort changeMask)
{
	mState = state;

	if(changeMask & WnckWindowState::WNCK_WINDOW_STATE_SKIP_TASKLIST)
		mGroup->updateStyle();
}