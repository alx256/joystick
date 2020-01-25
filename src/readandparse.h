#pragma once

class Console;
class ConsoleUI;

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "console.h"
#include "macros.h"
#include "joyfileproject.h"
#include "errors.h"
#include "executeprocess.h"

class JoyfileParser {
public:

    // Constructors
    JoyfileParser() {}
    JoyfileParser(std::string _path): path(_path) {}
    JoyfileParser(const JoyfileParser&) {}
    JoyfileParser& operator=(const JoyfileParser& jp);

    std::string getPath();
    std::string getError();

    bool parse(Console *parentConsole);
private:
    std::ifstream stream;

    std::string path, error;

    std::vector<std::string> keywords = {
        "use"
    };

    std::vector<std::string> instructionList, sourcesInstructionList;

    bool isKeyword(std::string instruction) {
        return std::find(keywords.begin(), keywords.end(), instruction)
               == keywords.end();
    }

    bool functionActions(std::string& instruction, Console* parentConsole);

    bool strAction(std::string& instruction);

    std::string line, instruction, finalInstruction, projectInstruction, functionInstruction, otherInstruction;
    unsigned int linePlace = 0;
    bool isFunction, isComment, isVariable, isParameter, isProject, event, hasStartedNewLine;
    std::string variable;

    JoyfileProject joyfileProject;

    std::ifstream lastCompileTimeStreamRead;
    std::time_t lastCompTime;
};

struct JoystickTotemLibrary {
    static std::map<std::string, int> functions;
    static std::map<std::string, int> projectFunctions;

    static bool isFunction(std::string instruction);
    static bool isProjectFunction(std::string instruction);
};
