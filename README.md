# Joystick #

Joystick is an easy to use build system

It is nowhere near complete, and there are still many things to be improved. However:

**Features**
- Program is built based on rules specified in the Joyfile
- A Joyfile is similar to a Makefile, but is easier to read and write
- Just create a project, specify its features and run the `joystick` command (the samples directory has some examples of Joyfiles)
- UI
- Scrolling (unstable)
- Optional autocomplete (unstable)
- Supports C++

# Installation #

Joystick requires the boost and the ncurses library

macOS (with Homebrew): `brew install boost ncurses`

Debian/Ubuntu: `sudo apt-get install libboost-all-dev libncurses5-dev libncursesw5-dev`

Use the build.sh script to build Joystick

`./build.sh`

By default, Joystick will be built using the curses library which can cause some character display problems on Linux. To fix this, you can build Joystick with ncursesw:

`./build --with-ncursesw`
