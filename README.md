# DisasterLauncher

## Windows
1. install [vcpkg](https://vcpkg.io/) and add to PATH
2. `git clone https://github.com/teamexeempire/disaster-launcher`
4. `cd disaster-launcher && vcpkg install`
5. `cmake -S . -B build && cmake --build build`

## Linux
1. Install following dependencies (as root):
    #### Arch Linux:
    `$ pacman -S git gcc cmake make sdl2 jansson curl`
    #### Ubuntu (and probably Debian):
    `$ apt install git gcc cmake make libsdl2-dev libjansson-dev libcurl4-gnutls-dev`
    #### Fedora (untested)
    `$ dnf install git gcc cmake make SDL2-devel libcurl-devel jansson-devel`

2. `git clone https://github.com/teamexeempire/disaster-launcher`
4. `cd disaster-launcher && vcpkg install`
5. `cmake -S . -B build && cmake --build build`