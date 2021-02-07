# Docklike Taskbar for XFCE

A modern, docklike, minimalist taskbar for XFCE

![screenshot](https://i.imgur.com/b4qDHCB.jpg)

## Internationalization

Contribute in localizing the plugin to your native tongue.

Simply go to the ```/po``` directory, and make your own ```[langcode].po``` file by editing ```Docklike.pot```. Then open a pull request.

## Build & Install

```bash
tar xvf xfce4-docklike-plugin-0.1.0.tar.gz && cd xfce4-docklike-plugin-0.1.0
./configure
make
sudo make install
```

Use `./configure --prefix=/usr/local` to change install location

### From git

```bash
git clone https://github.com/nsz32/docklike-plugin &&  cd docklike-plugin
./autogen.sh
make
sudo make install
```

Use `./autogen.sh --prefix=/usr/local` to change install location

### 3rd party packages

Provided by users. Submit a pull request if you have one for your distribution.

#### ArchLinux User Repository (AUR)

`yay -S xfce4-docklike-plugin-git`

#### Xubuntu 20.04 and above

```bash
sudo add-apt-repository ppa:xubuntu-dev/extras
sudo apt update
sudo apt install xfce4-docklike-plugin
```

#### Dependencies

+ libxfce4panel-2.0
+ libxfce4ui-2
+ gtk-3.0
+ cairo-1.16
+ libwnck-3.0
+ x11-1.6

## Features & Screenshots

Pinnable applications and grouped windows:

![screenshot](https://i.imgur.com/O0nvthj.jpg)

Use Ctrl to reorder your applications or access the settings panel (with right click):

![screenshot](https://i.imgur.com/CUFp6QP.jpg)

Fast application switching : by hovering mouse while button pressed or scrolling:

![screenshot](https://i.imgur.com/bpR1E6j.jpg)

Use your Super key to start or switch applications extremely fast:

![screenshot](https://i.imgur.com/OykcJlT.png)

Different indicator styles:

![screenshot](https://i.imgur.com/9nWqkCG.jpg)
