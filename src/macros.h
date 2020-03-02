//   Copyright 2020 Alexander Poulson
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#ifndef JOYSTICK_MACROS
#define JOYSTICK_MACROS

#define JOYSTICK_VERSION "beta-0.1"

#define JOYSTICK_CONSOLE_UI 	0
#define JOYSTICK_CONSOLE_TEXT	1

#define TOTEM_TYPE_INT          1
#define TOTEM_TYPE_BOOL         2
#define TOTEM_TYPE_FLOAT        3
#define TOTEM_TYPE_STRING       4
#define TOTEM_TYPE_LIST(type)   5 + type

#define CONSOLE_COLOR_RED 	1
#define CONSOLE_COLOR_BLUE 	2
#define CONSOLE_COLOR_CYAN 	3
#define CONSOLE_COLOR_GREEN 	4
#define CONSOLE_COLOR_GRAY	5

#endif
