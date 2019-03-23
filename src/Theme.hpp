#ifndef THEME_HPP
#define THEME_HPP

#include <iostream>
#include <string>

#include <gtk/gtk.h>

namespace Theme
{
	void init(GtkWidget* panel);
	void reload();
}

#endif