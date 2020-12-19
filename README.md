📌 Now support internationalization : **Contribute in localizing** the plugin to your tongue !

→ Simply go to the ```/po``` directory, and make your own ```[langcode].po``` file.



Docklike Taskbar for XFCE
-----------------------------------
A modern, docklike, minimalist taskbar for XFCE

![docklike-plugin](https://i.imgur.com/O0nvthj.jpg)

Build & Install
-----------------------------------

Docklike is published in
* ArchLinux User Repository (AUR) :  
`yay -S xfce4-docklike-plugin-git`

* Xubuntu 20.04 and above :  
```
sudo add-apt-repository ppa:xubuntu-dev/extras
sudo apt update
sudo apt install xfce4-docklike-plugin
```

* On other distros:
```
git clone https://github.com/nsz32/docklike-plugin && cd docklike-plugin
./autogen.sh
make
sudo make install
```

Dependencies :
libxfce4panel-2.0
libxfce4ui-2
gtk-3.0
cairo-1.16
libwnck-3.0
x11-1.6

Features
-----------------------------------
Pinnable applications and grouped windows (dock-like) :

![docklike-plugin](https://i.imgur.com/b4qDHCB.jpg)

Use Ctrl to reorder your applications or access the settings panel (with right click) :

![docklike-plugin](https://i.imgur.com/CUFp6QP.jpg)

Fast application switching : by hovering mouse while button pressed or scrolling :

![docklike-plugin](https://i.imgur.com/bpR1E6j.jpg)

Hotkeys : Use your Super key to start or switch applications extremely fast :

![docklike-plugin](https://i.imgur.com/OykcJlT.png)

Different styles :

![docklike-plugin](https://i.imgur.com/9nWqkCG.jpg)
