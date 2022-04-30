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

class Console;

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>
#include <filesystem>

#include "macros.h"
#include "joyfileproject.h"
#include "errors.h"
#include "executeprocess.h"
#include "log.h"

using namespace std::chrono_literals;

class JoyfileParser {
public:
    // Constructors
    JoyfileParser();
    ~JoyfileParser();

    inline std::string getPath()  { return path; }
    inline std::string getError() { return error; }

    bool parse();

    void clean();
    void setPath(const std::string& _path);

    // For installing the project
    std::map<std::string, std::string> getInstallLocation();
    std::map<std::string, std::string> getFileName();
private:
    inline int getPlatform() const {
        #if defined(__APPLE__) || defined(__MACH__)
            return 0;
        #elif defined(_WIN32) || defined(_WIN64)
            return 1;
        #elif defined(__linux__) || defined(linux) || defined(__linux)
            return 2;
        #endif

        return 3;
    }

    bool functionActions(std::string& instruction);

    bool strAction(std::string& instruction);

    std::ifstream stream;
    std::string path, rawPath, error;
    std::vector<std::string> instructionList, sourcesInstructionList;
    std::string line, instruction, finalInstruction, projectInstruction, platformInstruction, functionInstruction, otherInstruction, pathSimple;
    unsigned int linePlace, openedPlatformSpecifics;
    bool isFunction, isComment, isVariable, isParameter, isProject,
        isDarwin, isWindows, isLinux, isPlatformSpecific, event, hasStartedNewLine;
    std::string variable;
    JoyfileProject joyfileProject;
    std::ifstream lastCompileTimeStreamRead;
    std::time_t lastCompTime;
    std::map<std::string, std::string> installLocation, fileName;
};

struct JoystickTotemLibrary {
    static std::map<std::string, int> functions;
    static std::map<std::string, int> projectFunctions;

    static bool isFunction(std::string instruction);
    static bool isProjectFunction(std::string instruction);
};
