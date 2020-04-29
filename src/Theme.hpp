/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef THEME_HPP
#define THEME_HPP

#include <gtk/gtk.h>

#include <iostream>
#include <string>

#include "Dock.hpp"

namespace Theme
{
	void init();
	void load();

	void applyDefault(GtkWidget* widget);
} // namespace Theme

#endif