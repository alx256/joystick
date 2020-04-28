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

#include <fstream>
#include <string>
#include <map>

class Console {
public:
    Console(std::string _path, std::string _rawPath, int _id, bool _debug);

    // Function to start console
    virtual void start(std::string startingCommand) = 0;
    // Function to stop console
    virtual void stop() = 0;

    // The file that will be used for output
    std::ofstream outputLog;
    // The path of the source directory
    std::string path;
    // The path without the '/Joyfile' at the end
    std::string rawPath;

    // The function used by external functions to output to the log
    virtual void buildLog(const std::string& message, bool bold = false, int color = 0) = 0;

    // Used by the parser to set the install and file paths
    virtual void setInstallLocation(const std::string& _installLocation, const std::string& project) = 0;
    virtual void setFileName(const std::string& _fileName, const std::string& project) = 0;

    virtual void startLoadingBar(const size_t& maxSize) = 0;
    virtual void updateLoadingBar(const size_t& updateSize) = 0;

    // Handles command execution
    virtual bool execCommand(const std::string& command) = 0;

    // Debug mode enabled/disabled
    bool debug;
    int id;
};
