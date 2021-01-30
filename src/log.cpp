#include "log.h"

void Log::buildLog(const std::string& message, int color) {
//    log("\033[" + std::to_string(color) + ";1" + message + "\033[0m", message.size());
    log(message);
}

void Log::buildLogBold(const std::string& message, int color) { 
//    log("\033[" + std::to_string(color) + ";1;1m" + message + "\033[0m", message.size());
    log(message);
}

void Log::buildLogItalics(const std::string& message, int color) {
//    log("\033[" + std::to_string(color) + ";1;1m" + message + "\033[0m", message.size());
    log(message);
}

void Log::log(const std::string& message, LogType logType) {
    if (message.size() > 0) {
        switch (logType) {
        case INFO:
            std::cout << "[" << colored(102, 255, 255, "INFO") << "] ";
            break;
        case WARNING:
            std::cout << "[WARNING] ";
            break;
        case ERROR:
            std::cout << "[ERROR] ";
            break;
        }

        std::cout << message << '\n';
    } else
        std::cout << "";
}

std::string Log::colored(char r, char g, char b, const std::string& str) {
    std::stringstream ss;
    ss << "\33[38;2;" << r << ';' << g << ';' << b << 'm' << str << '[' << 'm';
    return ss.str();
}
