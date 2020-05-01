/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

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
} // namespace SettingsDialog

#endif // SETTINGS_HPP