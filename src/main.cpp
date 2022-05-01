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

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <utility>
#include <optional>

#include "readandparse.h"
#include "runcommand.h"
#include "log.h"

bool buildCommand = false,
    resetCommand = false,
    helpCommand = false,
    commandUsed = false,
    isNextArg = true;

std::string currentArg, nextArg;

void help() {
    std::cout <<
        "Usage: joystick <COMMAND> ...\n\n" <<
        "Commands:\n" <<
        "  build [PATH]      Builds using the Joyfile specified in PATH\n" <<
        "  reset             Clears build files\n" <<
        "  version           Displays version information\n" <<
        "  help              Displays this help information\n";
}

void version() {
    std::cout << "Joystick system version " << JOYSTICK_VERSION <<
                "\nAlexander Poulson 2019\n";
}

void build() {
    if (!isNextArg) {
        std::cerr << "[ERROR] No path specified\n";
        return;
    }

    std::cout << "[INFO] Starting build with joyfile " << nextArg << "/Joyfile\n";

    if (!std::filesystem::exists(nextArg + "/Joyfile")) {
        std::cerr << "[ERROR] Joyfile does not exist\n";
        return;
    }

    JoyfileParser parser;
    parser.setPath(nextArg);
    if (!parser.parse()) {
        std::cerr << "[ERROR] Error occured while reading Joyfile\n";
        std::cerr << parser.getError() << '\n';
    }
}

void reset() {
    if (!isNextArg) {
        std::cerr << "[ERROR] No path specified\n";
        return;
    }

    runCommand("rm -rf " + nextArg + "/.joystick");
    Log::log("Cleared build cache", INFO);
}

const std::vector<std::pair<std::string, std::function<void()>>> possibleCommands = {
    std::pair("build", std::bind(build)),
    std::pair("reset", std::bind(reset)),
    std::pair("version", std::bind(version)),
    std::pair("help", std::bind(help))
};

bool executeAction(int argc, const char ** argv) {
    currentArg = std::string(argv[1]);
    isNextArg = argc >= 3;
    if (isNextArg)
        nextArg = argv[2];
       
    std::string currentArgLocal = currentArg;
        
    // Check the first value of all the pairs in the vector to check for a match
    auto it = std::find_if(possibleCommands.begin(), possibleCommands.end(),
            [&currentArgLocal](const std::pair<std::string, std::function<void()>>& element) { return element.first == currentArgLocal; });

    if (it == possibleCommands.end()) {
        std::cerr << "[ERROR] Unknown command '" << currentArg << "'\n";
        return false;
    } else
        it->second();

    return true;
}

int main(int argc, const char ** argv) {
    if (argc <= 1) {
        help();
        return EXIT_SUCCESS;
    }

    if (!executeAction(argc, argv))
        return EXIT_FAILURE; 

    /*if (!boost::filesystem::exists(path + "/Joyfile")) {
        std::cerr << "Joyfile not found!\n";
        return 1;
    }*/

    // Prevents the path having two slashes. It will still work, but it looks nicer with only one.
    //Console console(path + std::string((path[path.size() - 1] != '/') ? "/Joyfile" : "Joyfile"), path);

    /*if (!execCommand.empty())
        console.start(execCommand);
    else
        console.start();*/
    
    return 0;
}
