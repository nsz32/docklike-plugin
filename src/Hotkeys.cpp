/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include "Hotkeys.hpp"

#define ModifierChange 85 //? this event isn't listed in libX11 ...

namespace Hotkeys
{
	bool mListening;
	bool mGrabing;

	bool mKeyAloneTrigger;

	int m1KeyCode;

	GdkFilterReturn globalEventHandler(GdkXEvent* gdk_xevent, GdkEvent* event, gpointer data)
	{
		XEvent* xevent = (XEvent*)gdk_xevent;

		switch (xevent->type)
		{
		case ModifierChange:
			if (GDK_MOD4_MASK & xevent->xkey.keycode)
			{
				mKeyAloneTrigger = true;
			}
			else if (mKeyAloneTrigger)
			{
				if (Settings::keyAloneActive)
					Wnck::switchToLastWindow(0);
				mKeyAloneTrigger = false;
			}
			break;
		case KeyPress:
			if (xevent->xkey.keycode >= m1KeyCode && xevent->xkey.keycode <= m1KeyCode + 9)
			{
				std::cout << "TRIGGER COMBO:" << (xevent->xkey.keycode - m1KeyCode) << std::endl;
				mKeyAloneTrigger = false;
			}
			break;
		}
		return GDK_FILTER_CONTINUE;
	}

	void init()
	{
		mListening = mGrabing = false;
		mKeyAloneTrigger = false;
		updateSettings();
	}

	void grabUngrab(bool grab)
	{
		// :)
	}

	void updateSettings()
	{
		Display* dpy = XOpenDisplay(0);
		m1KeyCode = (int)XKeysymToKeycode(dpy, XK_1);

		if (Settings::keyComboActive || Settings::keyAloneActive)
		{
			if (!mListening)
			{
				gdk_window_add_filter(NULL, globalEventHandler, NULL);
				mListening = true;
			}
		}
		else
		{
			if (mListening)
			{
				gdk_window_remove_filter(NULL, globalEventHandler, NULL);
				mListening = false;
			}
		}

		if (Settings::keyComboActive)
		{
			if (!mGrabing)
			{
				grabUngrab(true);
				mGrabing = true;
			}
		}
		else
		{
			if (mGrabing)
			{
				grabUngrab(false);
				mGrabing = false;
			}
		}
	}
} // namespace Hotkeys