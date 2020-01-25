#pragma once

class JoyfileParser;

#include <string>
#include <vector>

#include "console.h"

struct JoyfileProject {
    // Required
    std::string name, output_name, language;
    std::vector<std::string> sources;
    bool nothingMoreToGen = false;

    // Default
    std::string c_compiler = "gcc", cpp_compiler = "g++", python_interpreter = "python3",
                output_type = "binary";

    // Empty
    std::string extra_compilation_options, framework;
    std::vector<std::string> static_lib, shared_lib, include_paths, link_paths;

    Console* parentConsole = nullptr;
};
