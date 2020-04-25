#ifndef THEME_HPP
#define THEME_HPP

#include <gtk/gtk.h>

#include <iostream>
#include <string>

namespace Theme
{
	void init(GtkWidget* panel);
	void load();
} // namespace Theme

#endif