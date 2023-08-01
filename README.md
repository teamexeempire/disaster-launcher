# DisasterLauncher

## Windows
1. Download installer [Visual Studio 2022](https://visualstudio.microsoft.com/vs/community/).
2. Run installer and check **Desktop development with C++**
3. Install the shit
4. `git clone --recurse-submodules https://github.com/teamexeempire/disaster-launcher `
5. In Visual Studio 2022 click Open Folder or whatever its called
6. Select disasterlauncher as a target
7. do the build

## Linux
1. Install following dependencies (as root):
    #### Arch Linux:
    `$ pacman -S git base-devel gcc cmake make sdl2 jansson curl`
    #### Ubuntu (and probably Debian):
    `$ apt install git build-essential gcc cmake make libsdl2-dev libjansson-dev libcurl4-gnutls-dev`
    #### Fedora (untested)
    `$ dnf install git gcc cmake make SDL2-devel libcurl-devel jansson-devel`

2. `git clone --recurse-submodules https://github.com/teamexeempire/disaster-launcher `
4. `cd disaster-launcher && vcpkg install`
5. `cmake -S . -B build && cmake --build build`
