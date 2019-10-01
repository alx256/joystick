#pragma once

class JoyfileParser;
class Console;

#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <sstream>

#include <locale.h>
#ifdef USE_NCURSESW
#include <ncursesw/ncurses.h>
#else
#include <curses.h>
#endif
#include <boost/filesystem.hpp>

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
};
