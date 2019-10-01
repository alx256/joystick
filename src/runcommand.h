#pragma once

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <chrono>
#include <fstream>
#include <thread>

std::string runCommand(const char * command, bool debug = false);
