// ** opensource.org/licenses/GPL-3.0

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <iostream>

#include <string>
#include <vector>

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "Theme.hpp"
#include "Config.hpp"
#include "AppInfos.hpp"

extern "C"
{
	#include <libxfce4panel/libxfce4panel.h>
	#include <libxfce4ui/libxfce4ui.h>
}

#include "Dock.hpp"

namespace Plugin
{
	extern XfcePanelPlugin* mXfPlugin;
	extern Config* mConfig;
}

#endif