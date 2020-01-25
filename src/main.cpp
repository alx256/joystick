#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "consoleui.h"
#include "readandparse.h"

// The possible flags that can be specified as well as the Joyfile path
const std::vector<std::string> possibleFlags = {"--no-ui"},
// The possible flags that can be specified instead of the Joyfile path
                               singleArgvFlags = {"--version", "--help"},
// The possible flags that take in a value
                               possibleFlagVars = {"--exec"};

// The command that will be run on startup, based on the --exec flag
std::string execCommand;

int main(int argc, const char ** argv) {
    std::string path;
    if (argc < 2) {
        std::cerr << "You need to specifiy a path.\nIf your Joyfile is in your current directory, use '.'\n";

        return 1;
    } else {
        path = argv[1];
    }

    // Single use flags
    if (std::find(singleArgvFlags.begin(), singleArgvFlags.end(), std::string(argv[1])) != singleArgvFlags.end()) {
        std::string singleFlag = std::string(argv[1]);
        if (singleFlag == "--version") {
            std::cout << "Joystick system version " << JOYSTICK_VERSION <<
                      "\nAlexander Poulson 2019\n";
        } else if (singleFlag == "--help") {
            std::cout << "Usage: joystick <PATH> [FLAGS]\n\n" <<
                      "PATH:\n" <<
                      "The path to the Joyfile, if the Joyfile is in the current directory, use '.'\n\n" <<
                      "FLAGS:\n" <<
                      "--version		Displays version information\n" <<
                      "--help			Displays this help information\n" <<
                      "--no-ui			Runs text-based stdout console for build, rather than UI console\n" <<
                      "--exec=<COMMAND>	Joystick command that will be run as soon as the console is started.\n";
        }

        return 0;
    }

    std::string argvElement;
    for (int i = 1; i <= argc - 1; i++) {
        argvElement = std::string(argv[i]);
        if (std::find(possibleFlags.begin(), possibleFlags.end(), argvElement) != possibleFlags.end()) {
            if (i == 1) {
                // Not single use flag
                std::cerr << "You need to specify a path to a Joyfile first\n";
                return 1;
            }

            if (argvElement == "--no-ui") {
                // Text console
            }
        }

        auto loc = argvElement.find("=");

        if (loc != std::string::npos) {
            if (i == 1) {
                std::cerr << "You need to specify a path to a Joyfile first\n";
                return 1;
            }

            if (argvElement.substr(0, loc) == "--exec")
                execCommand = argvElement.substr(loc + 1, argvElement.size() - 1);
        }
    }

    if (!boost::filesystem::exists(path + "/Joyfile")) {
        std::cerr << "Joyfile not found!\n";
        return 1;
    }

    // Prevents the path having two slashes. It will still work, but it looks nicer with only one.
    ConsoleUI* console = new ConsoleUI(path + std::string((path[path.size() - 1] != '/') ? "/Joyfile" : "Joyfile"), path);

    if (!execCommand.empty())
        console->start(execCommand);
    else
        console->start();

    delete console;

    return 0;
}
