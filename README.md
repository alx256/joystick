# Joystick #

Joystick is an easy to use build system

# Installation #

Joystick requires the boost and the ncurses library

macOS (with Homebrew): `brew install boost ncurses`

Debian/Ubuntu: `sudo apt-get install libboost-all-dev libncurses5-dev libncursesw5-dev`

Use the build.sh script to build Joystick

`./build.sh`

By default, Joystick will be built using the curses library which can cause some character display problems on Linux. To fix this, you can build Joystick with ncursesw:

`./build --with-ncursesw`
