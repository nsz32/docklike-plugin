#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <gtk/gtk.h>

#include <string>

#include "Helpers.hpp"
#include "Settings.hpp"

extern "C"
{
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
}

#include "Plugin.hpp"

namespace SettingsDialog
{
	void popup();

	void close();
} // namespace SettingsDialog

#endif // SETTINGS_HPP