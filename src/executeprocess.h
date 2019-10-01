#pragma once

#include <string>
#include <sstream>
#include <chrono>
#include <fstream>
#include <thread>

#include <boost/filesystem.hpp>

#include "console.h"
#include "joyfileproject.h"
#include "errors.h"
#include "runcommand.h"
#include "macros.h"

bool execute(JoyfileProject project);
extern void getWarnings(std::string command);
extern void multithreadComp();
static bool compilationSuccess;
extern std::string errOutput;
static std::ofstream lastCompileTimeStream;
