// ** opensource.org/licenses/GPL-3.0

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include <iostream>
#include <string>
#include <vector>

#include "AppInfos.hpp"
#include "Keyboard.hpp"
#include "Settings.hpp"
#include "SettingsDialog.hpp"
#include "Theme.hpp"

extern "C"
{
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
}

#include "Dock.hpp"

namespace Plugin
{
	extern XfcePanelPlugin* mXfPlugin;
	extern GdkDevice* mPointer;

	void getPointerPosition(gint* x, gint* y);
	void aboutDialog();
} // namespace Plugin

#endif