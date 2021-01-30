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
class Console;

#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <sstream>

#include <unistd.h>

#include <locale.h>
#ifdef USE_NCURSESW
#include <ncursesw/ncurses.h>
#else
#include <curses.h>
#endif
#include <boost/filesystem.hpp>
#include <boost/asio/error.hpp>
#include <boost/system/error_code.hpp>

#include "console.h"
#include "readandparse.h"
#include "runcommand.h"
#include "macros.h"

class ConsoleUI: public Console {
public:
    ConsoleUI(std::string _path, std::string _rawPath);

    void start(std::string startingCommand = "") override;
    void stop() override;

    // Used for main loop
    bool running = true;
    // If user is in scroll mode
    bool scrolling = false;

    // Used by the parser to add live log
    void buildLog(const std::string& message, bool bold = false, int color = 0) override;

    // Used by the parser to set the install and file paths
    void setInstallLocation(const std::string& _installLocation, const std::string& project);
    void setFileName(const std::string& _fileName, const std::string& project);
    
    // Starts compilation loading bar
    void startLoadingBar(const size_t& maxSize) override;

    // Updates compilation loading bar
    void updateLoadingBar(const size_t& updateSize) override;
private:
    // The window at the bottom where the user enters commands
    WINDOW* inputWindow;
    // The main window that displays output
    WINDOW* logWindow;
    // The window above the input window, primarily used for the progress bar
    WINDOW* secondaryWindow;

    // The build log, each first element representing a line and the second element representing whether or not it needs a newline
    std::vector<std::pair<std::string, bool>> currentLog;
    // The attributes associated with each line of the build log
    std::vector<std::pair<bool, int>> currentLogAttrs;
    // For checking if a project name is valid
    std::vector<std::string> projects;
    // Used for scrolling
    int consoleLines = 0, startIndex, endIndex, startIndexPrev, endIndexPrev;

    // Clears the log
    void clearConsole();

    bool execCommand(const std::string& command) override;

    void getStr();
    // Checks if log needs to be scrolled
    void scrollIfNeeded(const int& y);
    // Runs less with the log
    void consoleLogScroll();
    // Adds a new message to our console log
    void consoleLog(std::string message, bool bold = false, int color = 0);
    // Runs ncurses functions that updates the log window
    void updateLog();
    // Displays help information on popupWindow
    void help(std::string doc = "");
    // Clears the generated cache
    void resetCache();

    // The input from the input window
    std::string command;

    // Boolean used to track whether or not the enter prompt should be displayed on secondaryWindow
    bool winStart = true;
    // Used for declaring ncurses windows
    int maxX, maxY;

    bool started = false, autocomplete = false;

    // The input string
    char str[20];

    // The line number
    int lineNum;

    // Used for controlling the loading bar
    size_t loadingSizeMax, loadingSizeCurr;

    // Used for installation
    std::map<std::string, std::string> installLocation, fileName;

    std::string proj, name, install;

    // The parser
    JoyfileParser parser;
};
