#pragma once

#include <string>
#include <iostream>
#include <sstream>

#include "macros.h"

enum LogType {
    NONE = 0,
    INFO,
    WARNING,
    ERROR
};

class Log {
public:
    static void buildLog(const std::string& message, int color);
    static void buildLogBold(const std::string& message, int color);
    static void buildLogItalics(const std::string& message, int color);
    static void log(const std::string& message, LogType logType = NONE);

private:
    static std::string colored(char r, char g, char b, const std::string& str);
};
