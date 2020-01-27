#include "readandparse.h"

bool JoyfileParser::parse(Console* parentConsole) {
    // Creates .joystick directory if necessary
    if (!boost::filesystem::exists(parentConsole->rawPath + "/.joystick")) {
        boost::filesystem::create_directory(parentConsole->rawPath + "/.joystick");
        boost::filesystem::create_directory(parentConsole->rawPath + "/.joystick/objects");
        boost::filesystem::create_directory(parentConsole->rawPath + "/.joystick/output");
    }

    stream = std::ifstream(path);

    joyfileProject.parentConsole = parentConsole;
    isParameter = false;
    isFunction = false;

    openedPlatformSpecifics = 0;

    auto buildTimeStart = std::chrono::high_resolution_clock::now();

    // Reads file line by line
    while(std::getline(stream, line)) {
        if (!isFunction) {
            functionInstruction.clear();
            instruction.clear();
        }

        if (!isProject)
            projectInstruction.clear();
        // Increases the line number (for error logging)
        ++linePlace;

        hasStartedNewLine = true;

        for (char y : line) {
            // Increases the column number (for error logging)
            isComment = false;

            // Ignores all spaces in front of a line when a new line is started
            if (hasStartedNewLine && y != ' ')
                hasStartedNewLine = false;

            // Sets isParameter when necessary so spaces can be appropriately ignored
            if (y == '"' && isFunction &&
                    (JoystickTotemLibrary::functions[finalInstruction] == TOTEM_TYPE_STRING
                     || JoystickTotemLibrary::projectFunctions[finalInstruction] == TOTEM_TYPE_STRING)) {
                isParameter = !isParameter;
            }

            // Spaces confuse things, so we ignore them
            if ((y == ' ' || y == '\t') && !isParameter)
                continue;

            if (hasStartedNewLine && y == ' ')
                continue;

            // Ignores comments
            if (line[0] == '#') {
                isComment = true;
            } else if (line[0] == ' ') {
                for (int i = 1; i <= line.size() - 1; ++i) {
                    if (line[i] == '#') {
                        isComment = true;
                    }
                }
            }

            // Ignores comments
            if (isComment)
                continue;

            std::string projName;

            // Checks if we are starting a project block
            if (!isProject) {
                if (y != '{') {
                    projectInstruction += y;
                } else {
                    if (projectInstruction.find("project") != std::string::npos && projectInstruction.find(":") != std::string::npos) {
                        projName = projectInstruction.substr(projectInstruction.find(":") + 1);
                        if (!strAction(projName)) {
                            error = Errors::unknownInstruction(projName, linePlace).getAll();
                            return false;
                        }
                        parentConsole->buildLog("== Name: " + projName + " ==");
                        joyfileProject.name = projName;

                        isProject = true;
                        projectInstruction.clear();
                        event = true;
                    } else {
                        error = Errors::unknownInstruction(projectInstruction, linePlace).getAll();
                        return false;
                    }
                }
            }

            // Checks if we are starting a platform block
            if (!isDarwin || !isWindows || !isLinux) {
                if (y != '[') {
                    platformInstruction += y;
                } else {
                    if ((platformInstruction.find("@darwin") != std::string::npos))
                        isDarwin = true;
                    if ((platformInstruction.find("@windows") != std::string::npos))
                        isWindows = true;
                    if ((platformInstruction.find("@linux") != std::string::npos))
                        isLinux = true;

                    isPlatformSpecific = true;
                    openedPlatformSpecifics++;

                    platformInstruction.clear();
                    event = true;
                }
            }

            if (isProject && y == '}') {
                isProject = false;
                projectInstruction.clear();
                event = false;

                // If some platform specific sections are still open (do not have an closing square bracket), return an error
                if (openedPlatformSpecifics != 0) {
                    error = Errors::missingEndSymbol("platform").getAll();
                    return false;
                }

                // We've reached the end of our project block, so now we must compile the project
                if (!execute(joyfileProject)) {
                    error = errOutput;
                    return false;
                }

                // Resets our project
                joyfileProject = JoyfileProject();
                joyfileProject.parentConsole = parentConsole;
                continue;
            } else if (!isProject && y == '}') {
                error = Errors::unknownInstruction("Bad Project End", linePlace).getAll();
            }

            if (isPlatformSpecific && y == ']') {
                isDarwin = false, isWindows = false, isLinux = false, isPlatformSpecific = false;
                openedPlatformSpecifics--;
                platformInstruction.clear();
                continue;
            } else if (!isPlatformSpecific && y == ']') {
                error = Errors::unknownInstruction("Bad Platform End", linePlace).getAll();
            }

            if (y == '(' && isFunction) {
                // If we are already in a function then this is ilegal
                error = Errors::missingEndSymbol("function", finalInstruction).getAll();
                return false;
            }

            // Checks if we are starting a function
            if (!isFunction) {
                if (y != '(') {
                    functionInstruction += y;
                } else {
                    event = true;

                    if (JoystickTotemLibrary::isFunction(functionInstruction) || (JoystickTotemLibrary::isProjectFunction(functionInstruction) && isProject)) {
                        isFunction = true;
                        finalInstruction = functionInstruction;
                    } else {
                        error = Errors::unknownInstruction(functionInstruction, linePlace).getAll();
                        return false;
                    }
                }
            } else {
                if (y == ')') {
                    isFunction = false;
                    event = false;

                    // Check if the function is only meant to be run for a specific platform
                    if (isPlatformSpecific) {
                        if (isDarwin && (getPlatform() != 0)) {
                            instruction.clear();
                            continue;
                        }

                        if (isWindows && (getPlatform() != 1)) {
                            instruction.clear();
                            continue;
                        }

                        if (isLinux && (getPlatform() != 2)) {
                            instruction.clear();
                            continue;
                        }
                    }

                    if (!functionActions(instruction, parentConsole))
                        return false;

                    if (finalInstruction == "log") {
                        parentConsole->buildLog(instruction);
                    } else if (finalInstruction == "output_name") {
                        joyfileProject.output_name = instruction;
                    } else if (finalInstruction == "output_type") {
                        joyfileProject.output_type = instruction;
                    } else if (finalInstruction == "language") {
                        joyfileProject.language = instruction;
                    } else if (finalInstruction == "static_lib") {
                        joyfileProject.static_lib = instructionList;
                    } else if (finalInstruction == "shared_lib") {
                        joyfileProject.shared_lib = instructionList;
                    } else if (finalInstruction == "extra_compilation_options") {
                        joyfileProject.extra_compilation_options = instruction;
                    } else if (finalInstruction == "sources") {
                        sourcesInstructionList.clear();

                        if (boost::filesystem::exists(parentConsole->rawPath + "/.joystick/objects/" + joyfileProject.name + "/dat/.last_comp_time")) {
                            lastCompileTimeStreamRead.open(parentConsole->rawPath + "/.joystick/objects/" + joyfileProject.name + "/dat/.last_comp_time");
                            lastCompileTimeStreamRead >> lastCompTime;
                            lastCompileTimeStreamRead.close();

                            // Check if a source file needs to be recompiled
                            for (int i = 0; i <= instructionList.size() - 1; i++) {
                                pathSimple = instructionList[i].substr(instructionList[i].find_last_of("/") + 1, instructionList[i].size() - 1);
                                if ((std::difftime(lastCompTime, boost::filesystem::last_write_time(parentConsole->rawPath + '/' + instructionList[i])) <= 0) ||
                                        !boost::filesystem::exists(parentConsole->rawPath + "/.joystick/objects/" + joyfileProject.name + '/' + 
                                        pathSimple.substr(0, pathSimple.find_first_of(".")) + ".o"))
                                    sourcesInstructionList.push_back(instructionList[i]);
                            }
                            if (sourcesInstructionList.empty())
                                joyfileProject.nothingMoreToGen = true;
                            else
                                joyfileProject.nothingMoreToGen = false;
                        } else {
                            sourcesInstructionList = instructionList;
                        }

                        joyfileProject.sources = sourcesInstructionList;
                    } else if (finalInstruction == "include_paths") {
                        joyfileProject.include_paths = instructionList;
                    } else if (finalInstruction == "link_paths") {
                        joyfileProject.link_paths = instructionList;
                    } else if (finalInstruction == "framework") {
                        joyfileProject.framework = instruction;
                    } else if (finalInstruction == "cpp_compiler") {
                        joyfileProject.cpp_compiler = instruction;
                    }

                    instruction.clear();
                    continue;
                } else {
                    instruction += y;
                }
            }

            if (!event)
                otherInstruction += y;
            else
                otherInstruction.clear();
        }

        if (!isFunction && !otherInstruction.empty()) {
            error = Errors::unknownInstruction(otherInstruction, linePlace).getAll();
            return false;
        }
    }

    // Checks there is an illegal missing end symbol
    if (isFunction) {
        error = Errors::missingEndSymbol("function", finalInstruction).getAll();
        return false;
    } else if (isProject) {
        error = Errors::missingEndSymbol("project").getAll();
        return false;
    }

    auto buildTimeEnd = std::chrono::high_resolution_clock::now();

    std::ostringstream finalTime;
    finalTime << '(' <<
              std::chrono::duration_cast<std::chrono::milliseconds>(buildTimeEnd - buildTimeStart).count() <<
              " ms)";

    parentConsole->buildLog("Completed successfully " + finalTime.str(), true, CONSOLE_COLOR_GREEN);

    return true;
}

bool JoyfileParser::functionActions(std::string& instruction, Console* parentConsole) {
    switch(JoystickTotemLibrary::functions[finalInstruction]) {
    case TOTEM_TYPE_STRING:
        return strAction(instruction);
    case TOTEM_TYPE_INT:
        for (char y : instruction) {
            if (!std::isdigit(y)) {
                error = Errors::invalidParameter(instruction, linePlace).getAll();
                return false;
            }
        }
        return true;
    case TOTEM_TYPE_BOOL:
        // TODO: Add boolean support
        break;
    case TOTEM_TYPE_FLOAT:
        // TODO: Add float support
        break;
    }

    if (isProject) {
        instructionList.clear();

        std::stringstream ss(instruction);
        std::string tempStr;

        switch(JoystickTotemLibrary::projectFunctions[finalInstruction]) {
        case TOTEM_TYPE_STRING:
            return strAction(instruction);
        case TOTEM_TYPE_INT:
            // TODO: Add integer support
            break;
        case TOTEM_TYPE_BOOL:
            // TODO: Add boolean support
            break;
        case TOTEM_TYPE_FLOAT:
            // TODO: Add float support
            break;
        case TOTEM_TYPE_LIST(TOTEM_TYPE_STRING):
            boost::split(instructionList, instruction, boost::is_any_of(","));
            for (std::string& instr : instructionList) if (!strAction(instr)) return false;
            return true;
        }
    }

    error = Errors::invalidParameter(instruction, linePlace).getAll();
    return false;
}

bool JoyfileParser::strAction(std::string& instruction) {
    int counter = 0;

    for (char y : instruction)
        if (y == '"') counter++;

    if ((instruction[0] == '"') && (instruction[instruction.size() - 1] == '"') && counter <= 2) {
        instruction.erase(0, 1);
        instruction.erase(instruction.size() - 1, instruction.size());
        return true;
    } else {
        error = Errors::invalidParameter(instruction, linePlace).getAll();
        return false;
    }
}

std::string JoyfileParser::getError()                   {
    return error;
}
std::string JoyfileParser::getPath()                    {
    return path;
}

bool JoystickTotemLibrary::isFunction(std::string instruction)          {
    return functions.find(instruction) != functions.end();
}
bool JoystickTotemLibrary::isProjectFunction(std::string instruction)   {
    return projectFunctions.find(instruction) != projectFunctions.end();
}

std::map<std::string, int> JoystickTotemLibrary::functions = {
    std::make_pair("log", TOTEM_TYPE_STRING)
};

std::map<std::string, int> JoystickTotemLibrary::projectFunctions = {
    std::make_pair("output_name", TOTEM_TYPE_STRING),
    std::make_pair("output_type", TOTEM_TYPE_STRING),
    std::make_pair("language", TOTEM_TYPE_STRING),
    std::make_pair("sources", TOTEM_TYPE_LIST(TOTEM_TYPE_STRING)),
    std::make_pair("static_lib", TOTEM_TYPE_LIST(TOTEM_TYPE_STRING)),
    std::make_pair("shared_lib", TOTEM_TYPE_LIST(TOTEM_TYPE_STRING)),
    std::make_pair("extra_compilation_options", TOTEM_TYPE_STRING),
    std::make_pair("include_paths", TOTEM_TYPE_LIST(TOTEM_TYPE_STRING)),
    std::make_pair("link_paths", TOTEM_TYPE_LIST(TOTEM_TYPE_STRING)),
    std::make_pair("framework", TOTEM_TYPE_STRING),
    std::make_pair("cpp_compiler", TOTEM_TYPE_STRING)
};
