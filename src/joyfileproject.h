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
    std::string extra_compilation_options, framework, install_location;
    std::vector<std::string> static_lib, shared_lib, include_paths, link_paths;

    Console* parentConsole = nullptr;
};
