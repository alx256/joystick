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

#pragma once

#include <string>
#include <sstream>
#include <chrono>
#include <fstream>
#include <thread>
#include <filesystem>

#include "joyfileproject.h"
#include "errors.h"
#include "runcommand.h"
#include "macros.h"
#include "directory.h"
#include "log.h"

bool execute(JoyfileProject project);
extern void getWarnings(std::string command);
extern void multithreadComp(bool warnings = true);
static bool compilationSuccess;
extern std::string errOutput;
extern JoystickDirectory dir;
static std::ofstream lastCompileTimeStream;
