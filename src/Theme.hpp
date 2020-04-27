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