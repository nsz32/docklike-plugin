/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#include <libxfce4panel/libxfce4panel.h>

extern void construct(XfcePanelPlugin* xfPlugin);
XFCE_PANEL_PLUGIN_REGISTER(construct);