#pragma once

class JoyfileParser;

#include <fstream>
#include <string>

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

    virtual void startLoadingBar(const size_t& maxSize) = 0;
    virtual void updateLoadingBar(const size_t& updateSize) = 0;

    // Handles command execution
    virtual bool execCommand(const std::string& command) = 0;

    // Debug mode enabled/disabled
    bool debug;
    int id;
protected:
    // The parser
    JoyfileParser* parser;
};
