# Game01

[![Game01](/gitimages/Screenshot_0001.png)](#)  

Short description will go here.

## Pre-compiled binaries

Pre-compiled binaries can be found in the releases tab (currently only for Windows, using Direct3D).

## Building

### Dependencies

- [Urho3D](https://urho3d.github.io/)

Always refer to the source documentation in [Building Urho3D library](https://urho3d.github.io/documentation/1.7.1/_building.html)

The following is mostly a way to remember the process.

Get the latest sources from GitHub
```sh
$ git clone -b master https://github.com/urho3d/Urho3D
```
or download and extract the latest stable release (currently 1.7.1).  
If you get compilation errors when building the master branch, use the latest stable release.

Assuming the following folders:
- <URHO3D_SOURCE> : the folder where the library sources are extracted (e.g. ~/Urho3D)
- <URHO3D_INSTALL> : the folder where the library will be installed (e.g. ~/Libs/Urho3D)
___
- Linux
```sh
$ cd <URHO3D_SOURCE>
$ mkdir build
$ cd build
$ cmake ../ -DURHO3D_LIB_TYPE=SHARED -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<URHO3D_INSTALL>
$ make
$ make install
```
You will probably need to install additional libraries in order to build Urho3D. Depending on the distribution used I've needed to install:
- pulseaudio-libs-devel
- mesa-libGL-devel (this will install libX11-devel, libXext-devel, libXxf86vm-devel)

I didn't installed the following ones:
- libXrandr-devel (this will install libXrender-devel)
- libXcursor-devel - X11 cursor management library
- libXi-devel - X11 input extension library
- libXinerama-devel
- libXScrnSaver-devel
___
- MinGW
```sh
$ cd <URHO3D_SOURCE>
$ mkdir build
$ cd build
$ cmake ../ -G"MinGW Makefiles" -DMINGW=1 -DURHO3D_LIB_TYPE=SHARED -DCMAKE_BUILD_TYPE=Release -DMINGW_PREFIX=<PATH_TO_MINGW_BIN> -DCMAKE_INSTALL_PREFIX=<URHO3D_INSTALL>
$ mingw32-make
$ mingw32-make install
```

If the MinGW binaries are in the PATH environment variable, the MINGW_PREFIX should not be used.
___
- Visual Studio 2012
```sh
$ cd <URHO3D_SOURCE>
$ mkdir build
$ cd build
$ cmake ../ -G"Visual Studio 11 2012" -DVS=11 -DURHO3D_LIB_TYPE=SHARED -DCMAKE_INSTALL_PREFIX=<URHO3D_INSTALL>
$ cmake --build . --target ALL_BUILD --config Release
$ cmake --build . --target INSTALL --config Release
```
___
If the library builds and installs correctly, the folder <URHO3D_SOURCE> can now be deleted.  
If you plan to use the library yourself, don't delete it. The source code is great documentation.

### Building the Game01 demo

Assuming the following folders:
- <GAME01_INSTALL> : the folder where the demo will be installed (e.g. ~/Games/Game01)

Clone the project to the local hard drive.
```sh
$ git clone -b master https://gitlab.com/pflat/Game01
```

Build and install the demo
- Linux
```sh
$ cd Game01
$ mkdir build
$ cd build
$ cmake ../ -DCMAKE_BUILD_TYPE=Release -DURHO3D_HOME=<URHO3D_INSTALL> -DCMAKE_INSTALL_PREFIX=<GAME01_INSTALL>
$ make
$ make install
```

- MinGW
```sh
$ cd Game01
$ mkdir build
$ cd build
$ cmake ../ -G"MinGW Makefiles" -DMINGW=1 -DCMAKE_BUILD_TYPE=Release -DURHO3D_HOME=<URHO3D_INSTALL> -DCMAKE_INSTALL_PREFIX=<GAME01_INSTALL>
$ mingw32-make
$ mingw32-make install
```

- Visual Studio 2012
```sh
$ cd Game01
$ mkdir build
$ cd build
$ cmake ../ -G"Visual Studio 11 2012" -DURHO3D_HOME=<URHO3D_INSTALL> -DCMAKE_INSTALL_PREFIX=<GAME01_INSTALL>
$ cmake --build . --target Game01 --config Release
$ cmake --build . --target INSTALL --config Release
```

Or, after step 4, open the project created by CMake with Visual Studio 2012, and build from the IDE.

## Keeping the code up-to-date

```sh
$ cd Game01
$ git pull origin master
```

Recompile, delete previous install and reinstall.

## Running

- Linux
```sh
$ cd <GAME01_INSTALL>
$ ./Game01
```

- Windows
```sh
$ cd <GAME01_INSTALL>
$ Game01.exe
```

## Controls

- In space:
  - W / S - Ship pitch
  - A / D - Ship yaw
  - Q / E - Ship roll
  - Keypad+ / Keypad- - Ship speed
  - TAB - Full speed
  - BACKSPACE - Stop
  - PageUp / PageDown - Change ship

- On ground:
  - W / S - Walk forward / backward
  - A / D - Turn left / right
  - Q / E - Strafe left / right
  - SPACE - Jump
  - SHIFT - Toogle walk / run

- General
  - Ctrl + TAB - Switch active scene (space and ground)
  - Mouse move + RMB - Move camera
  - Mouse wheel - Zoom camera
  - C - Reset camera position
  - F - Toogle camera mode (follow and free)
    In free mode, control the camera with:
    - I / K - Move forward / backward
    - J / L - Strafe left / right
    - U / O - Roll left / right
  - PrtScn - Take screenshot
  - ESC - Exit

- Debug:
  - Ctrl + F1 - Cycle texture quality
  - Ctrl + F2 - Cycle material quality
  - Ctrl + F3 - Toggle specular lighting
  - Ctrl + F4 - Toggle shadow rendering
  - Ctrl + F5 - Cycle shadow resolution
  - Ctrl + F6 - Cycle shadow filtering quality
  - Ctrl + F7 - Toggle occlusion culling
  - Ctrl + F8 - Toggle dynamic instancing
  - Ctrl + F9 - Toogle draw debug geometry
  - Ctrl + F10 - Toggle debug HUD (not implemented yet)
