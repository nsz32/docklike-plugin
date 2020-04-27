#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <gtk/gtk.h>

extern "C"
{
#include <libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
}

#include "Plugin.hpp"

namespace Settings
{
	GtkWidget* popup();
} // namespace Settings

#endif // SETTINGS_HPP