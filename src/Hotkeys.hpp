/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <iostream>
#include <string>

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include "Settings.hpp"

namespace Hotkeys
{
	void init();
	void updateSettings();
} // namespace Hotkeys

#endif