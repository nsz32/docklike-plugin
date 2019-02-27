CC = g++

CFLAGS = -g $(shell pkg-config --cflags libxfce4panel-2.0 libxfce4ui-2 gtkmm-3.0 libwnck-3.0)
LDLIBS = $(shell pkg-config --libs libxfce4panel-2.0 libxfce4ui-2 gtkmm-3.0 libwnck-3.0)
DEFINES = -DPACKAGE_VERSION="0.1" -DPACKAGE_NAME="\"nmt\"" -DGETTEXT_PACKAGE="\"gettext_package\"" -DPACKAGE_LOCALE_DIR="\"/\"" -DWNCK_I_KNOW_THIS_IS_UNSTABLE

all : libnmt.so

libnmt.so:o/register.o o/Plugin.o o/Group.o o/Taskbar.o o/Window.o o/XDG.o o/Utility.o
	$(CC) $(LDLIBS) -Wl,--no-undefined -shared -o libnmt.so o/*.o


o/XDG.o: src/XDG.hpp src/XDG.cpp
	$(CC) $(DEFINES) $(CFLAGS) -fPIC -o $@ -c src/XDG.cpp

o/%.o: src/%.cpp
	$(CC) $(DEFINES) $(CFLAGS) -fPIC -o $@ -c $<


o/register.o:
	gcc $(CFLAGS) -fPIC -o $@ -c src/register.c

clean:
	rm -f o/*.o *.so

install:
	cp libnmt.so /usr/lib/xfce4/panel/plugins/
	cp nmt.desktop /usr/share/xfce4/panel/plugins/

uninstall:
	rm /usr/lib/xfce4/panel/plugins/libnmt.so
	rm /usr/share/xfce4/panel/plugins/nmt.desktop