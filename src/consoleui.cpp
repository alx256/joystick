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

#include "consoleui.h"

ConsoleUI::ConsoleUI(std::string _path, std::string _rawPath) : Console(_path, _rawPath, JOYSTICK_CONSOLE_UI /*ID*/, false /*Debug mode*/) {}

using namespace std::chrono;

void ConsoleUI::start(std::string startingCommand) {
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    start_color();
    use_default_colors();

    // -1 for default background
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_BLUE, -1);
    init_pair(3, COLOR_CYAN, -1);
    init_pair(4, COLOR_GREEN, -1);
    init_color(COLOR_WHITE, 134, 134, 134); // Make the colour white a light gray, to be used by the autocomplete feature
    init_pair(5, COLOR_WHITE, -1);

    maxX = getmaxx(stdscr);
    maxY = getmaxy(stdscr);

    startIndex = 0;

    inputWindow = newwin(1, maxX, maxY - 1, 0);
    logWindow = newwin(maxY - 2, maxX, 0, 0);
    secondaryWindow = newwin(1, maxX, maxY - 2, 0);

    consoleLog("Joystick version " + std::string(JOYSTICK_VERSION));
    consoleLog("Using Joyfile: " + parser->getPath());

    // Log to be used for parser output
    time_t currTime = system_clock::to_time_t(system_clock::now());
    outputLog.open(std::string(rawPath + "/.joystick/output/Joystick_Output") + std::string(std::ctime(&currTime)));

    bool lastDebug = debug;
    char debugStatus[21] = "Debug mode set to: ";
    int scrollKey, scrollPosX = 0, scrollPosY = 0;
    keypad(logWindow, true); // For arrow key navigation
    keypad(inputWindow, true); // For getStr()
    while (running) {
        updateLog();
        if (!scrolling) {
            debugStatus[19] = (debug) ? '1' : '0';
            debugStatus[20] = 0x00;
            wclear(secondaryWindow);
            wclear(inputWindow);
            if (winStart && lastDebug == debug)
                wprintw(secondaryWindow, "Enter a command. Type 'help' for help");
            else if (lastDebug != debug)
                wprintw(secondaryWindow, debugStatus);
            else
                wprintw(secondaryWindow, "Unknown command");

            lastDebug = debug;
            wrefresh(secondaryWindow);

            wmove(inputWindow, 0, 0);
            wprintw(inputWindow, ":");
            wrefresh(inputWindow);

            if (startingCommand.empty()) {
                if (autocomplete)
                    getStr();
                else {
                    wgetstr(inputWindow, str);
                    command = std::string(str);
                }
            }

            if (!execCommand((startingCommand.empty()) ? command : startingCommand)) {
                startingCommand.clear();
                continue;
            }

            startingCommand.clear();

            winStart = true;
        } else {
            if (scrollPosY < 0) {
                scrollPosY = 0;
                if (startIndex > 0) {
                    startIndex--;
                    endIndex--;
                }
            }

            if (scrollPosY > getmaxy(logWindow) - 1) {
                scrollPosY = getmaxy(logWindow) - 1;
                if (startIndex < currentLog.size() - 1) {
                    startIndex++;
                    endIndex++;
                }
            }

            wmove(logWindow, scrollPosY, scrollPosX);
            wclear(secondaryWindow);
            wprintw(secondaryWindow, "SCROLL MODE (q to quit)");
            wrefresh(secondaryWindow);

            scrollKey = wgetch(logWindow);

            switch(scrollKey) {
            case KEY_RIGHT:
            case 0x6C:
                if (scrollPosX < maxX)
                    scrollPosX++;
                break;
            case KEY_LEFT:
            case 0x68:
                if (scrollPosX > 0)
                    scrollPosX--;
                break;
            case KEY_UP:
            case 0x6B:
                scrollPosY--;
                break;
            case KEY_DOWN:
            case 0x6A:
                if (scrollPosY < currentLog.size() - 1)
                    scrollPosY++;
                break;
            case 0x71:
                scrolling = false;
                startIndex = startIndexPrev;
                endIndex = endIndexPrev;
                break;
            }
        }
    }

    delete parser;
    stop();
}

const std::vector<std::string> commands = {
    "start",
    "quit",
    "help",
    "scroll",
    "debug",
    "clear",
    "reset",
    "autocomplete"
};

bool found = false;

void ConsoleUI::getStr() {
    int c = 0;

    memset(str, 0, sizeof(str));

    while (c != 10) {
        found = false;

        if (c == KEY_BACKSPACE && strlen(str) + 1 > 0) {
            str[strlen(str) - 1] = 0;
            wclear(inputWindow);
            wprintw(inputWindow, ":");
        } else {
            str[strlen(str)] = c;
        }

        for (std::string command : commands) {
            if (std::string(str) == command.substr(0, strlen(str)) && strlen(str) != 0 && !found) {
                found = true;
                wmove(inputWindow, 0, 1);
                wattron(inputWindow, COLOR_PAIR(CONSOLE_COLOR_GRAY));
                wprintw(inputWindow, command.c_str());
                wattroff(inputWindow, COLOR_PAIR(CONSOLE_COLOR_GRAY));
            } else if (found)
                continue;
            else {
                found = false;
                wclear(inputWindow);
                wprintw(inputWindow, ":");
                wmove(inputWindow, 0, 1);
            }
        }

        wmove(inputWindow, 0, 1);
        wprintw(inputWindow, str);
        c = wgetch(inputWindow);
    }

    command = std::string(str);
}

void ConsoleUI::clearConsole()  {
    currentLog.clear();
    currentLogAttrs.clear();

    startIndex = startIndexPrev;
    endIndex = endIndexPrev;

    wclear(logWindow);
    wrefresh(logWindow);
}

void ConsoleUI::help(std::string doc)  {
    clearConsole();
    std::stringstream helpText;
    if (doc.empty()) {
        helpText << "== HELP ==\n" <<
                 "start:\t\t\tStarts the build using instructions from the Joyfile\n" 		<<
                 "quit:\t\t\tExits joystick\n" 							<<
                 "clear:\t\t\tClears the current console log\n" 					<<
                 "reset:\t\t\tResets the build cache\n" 						<<
                 "scroll:\t\t\tEnter scroll mode, for more information type 'help scroll'\n"	<<
                 "debug:\t\t\tEnter debug mode, for more information type 'help debug'\n"	<<
                 "autocomplete:\t\tEnter autocomplete mode, *unstable*\n";
    } else if (doc == " scroll") {
        helpText << "== HELP: SCROLL ==\n"							<<
                 "Enters scroll mode, allowing you to navigate though the build log.\n\n"	<<
                 "Key bindings:\n"								<<
                 "Arrow keys / hjkl:\t\t\tMove the cursor\n"					<<
                 "q:\t\t\t\t\tExit scroll mode\n";
    } else if (doc == " debug") {
        helpText << "== HELP: DEBUG ==\n"							<<
                 "Enters debug mode, adding additional build information.\n"			<<
                 "The build output becomes more verbose, including the id of the thread which is executing which command and the shell command which is being executed\n";
    } else {
        consoleLog("Couldn't find any related help information");
        return;
    }
    consoleLog(helpText.str());
}

void ConsoleUI::stop()  {
    endwin();
}

void ConsoleUI::consoleLog(std::string message, bool bold, int color) {
    std::stringstream ss(message);
    std::string line;
    bool done;

    while (std::getline(ss, line)) {
        done = false;

        while (!done) {
            // Newline due to text not fiting on screen
            if (line.size() > maxX) {
                currentLog.push_back(std::make_pair(line.substr(0, maxX), false));
                line = line.substr(maxX, line.size() - 1);
            } else {
                currentLog.push_back(std::make_pair(line, true));
                done = true;
            }
            currentLogAttrs.push_back(std::make_pair(bold, color));
            endIndex = currentLog.size() - 1;
            lineNum++;
        }

        scrollIfNeeded(getmaxy(logWindow));
    }
}

bool ConsoleUI::execCommand(const std::string& command) {
    if (command == "quit") {
        running = false;
    } else if (command == "start") {
        clearConsole();

        delete parser; // Free parser memory

        // Clean parser and reallocate memory
        Console::parser = new JoyfileParser(Console::path);

        consoleLog("Starting parsing process");

        if (!parser->parse(this))
            consoleLog("Error: " + parser->getError(), true, CONSOLE_COLOR_RED);
    } else if (command.substr(0, 7)  == "install") {
        clearConsole();

        if (command.size() <= 8) {
            consoleLog("No install path specified!");
        } else {
            bool valid = false;
            std::string projName = command.substr(8, command.size());
            for (const std::string& project : projects) {
                if (project == projName) {
                    valid = true;
                    break;
                }
            }

            if (!valid) {
                consoleLog("Invalid project name: " + projName);
                return false;
            }

            install = installLocation[projName];
            name = fileName[projName];

            consoleLog("Installing to path " + install);

            // Check if target directory is writable by seeing if creating a directory
            // iterator fails or not
            // If it is writable, just perform the copy action
            // If not, exit the window to provide the user to enter their password
            endwin();
            std::system(std::string("sudo cp " + name + " " + install + " 2>&1").c_str());
            std::cout << "Press ENTER to continue\n";
            std::cin.ignore();
        }
    } else if (command == "clear") {
        clearConsole();
    } else if (command == "scroll") {
        consoleLogScroll();
    } else if (command.substr(0, 4) == "help") {
        help(command.substr(4, command.size()));
    } else if (command == "reset") {
        resetCache();
    } else if (command == "debug") {
        debug = !debug;
    } else if (command == "autocomplete") {
        autocomplete = !autocomplete;
    } else {
        winStart = false;
        return false;
    }

    return true;
}

void ConsoleUI::buildLog(const std::string& message, bool bold, int color)  {
    consoleLog(message, bold, color);
    time_t currTime = system_clock::to_time_t(system_clock::now());
    outputLog << '[' << currTime << "] " << message << '\n';
    updateLog();
}

void ConsoleUI::setInstallLocation(const std::string& _installLocation, const std::string& project) {
    projects.push_back(project);
    installLocation[project] = _installLocation;
}

void ConsoleUI::setFileName(const std::string& _fileName, const std::string& project) { fileName[project] = _fileName; }

void ConsoleUI::scrollIfNeeded(const int& y) {
    // Scroll required
    while ((currentLog.size() - startIndex) > y)
        startIndex++;
}

void ConsoleUI::consoleLogScroll() {
    startIndexPrev = startIndex;
    endIndexPrev = endIndex;
    scrolling = true;
}

void ConsoleUI::updateLog() {
    wmove(logWindow, 0, 0);
    wclear(logWindow);
    for (int i = startIndex; i <= endIndex; i++) {
        std::pair<std::string, bool> line = currentLog[i];
        if (line.second)
            line.first += std::string("\n");

        if (currentLogAttrs[i].first)
            wattron(logWindow, A_BOLD);

        if (currentLogAttrs[i].second != 0)
            wattron(logWindow, COLOR_PAIR(currentLogAttrs[i].second));

        wprintw(logWindow, line.first.c_str());

        if (currentLogAttrs[i].first)
            wattroff(logWindow, A_BOLD);

        if (currentLogAttrs[i].second != 0)
            wattroff(logWindow, COLOR_PAIR(currentLogAttrs[i].second));
    }
    wrefresh(logWindow);
}

void ConsoleUI::startLoadingBar(const size_t& maxSize) {
    loadingSizeMax = 0;
    loadingSizeCurr = 0;

    loadingSizeMax = maxSize;
    wclear(secondaryWindow);
    wrefresh(secondaryWindow);
}

void ConsoleUI::updateLoadingBar(const size_t& updateSize) {
    loadingSizeCurr += updateSize;

    float fill = ((float)loadingSizeCurr / (float)loadingSizeMax) * ((float)getmaxx(stdscr));

    wprintw(secondaryWindow, std::string(fill, '=').c_str());
    wrefresh(secondaryWindow);
}

void ConsoleUI::resetCache() {
    clearConsole();
    std::string cachePath = rawPath + "/.joystick";
    consoleLog("Resetting joystick cache...");
    if (boost::filesystem::exists(cachePath))
        consoleLog(runCommand(std::string("rm -rf " + cachePath).c_str(), debug));
    else {
        consoleLog("Couldn't find cache folder");
        return;
    }

    consoleLog("Completed successfully");
}
