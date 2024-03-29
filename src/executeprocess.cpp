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

#include "executeprocess.h"

std::string errOutput, errOutputCommand, _rawPath, compOut;
std::string warnings;
JoyfileProject project;
JoystickDirectory dir;

std::ifstream errOutputStream;
std::thread compilationThread;
std::stringstream command;

void getWarnings(std::string command) {
    warnings = runCommand(command.c_str(), false);
}

void multithreadComp(bool warnings) {
    compilationThread = std::thread(getWarnings,
            (warnings) ? std::string(command.str() + " -Wall -Wextra 2>&1") : command.str());
    compOut = runCommand(std::string(command.str() + 
            ((warnings) ? " -w " : "") + "2>" + _rawPath + "/.joystick/output/.comp_out").c_str(), false);

    errOutputStream.open(_rawPath + "/.joystick/output/.comp_out");

    errOutputCommand.assign(std::istreambuf_iterator<char>(errOutputStream),
                            std::istreambuf_iterator<char>());

    compilationThread.join();
}

bool execute(JoyfileProject _project) {
    project = _project;

    command.str("");

    if (errOutputStream.is_open())
        errOutputStream.close();

    errOutputStream.open(_rawPath + "/.joystick/output/.comp_out");

    if (project.c_compiler.empty()) {
        errOutput = Errors::missingDefinition("c_compiler").getAll();
        return false;
    } else if (project.cpp_compiler.empty()) {
        errOutput = Errors::missingDefinition("cpp_compiler").getAll();
        return false;
    } else if (project.output_name.empty()) {
        errOutput = Errors::missingDefinition("output_name").getAll();
        return false;
    } else if (project.output_type.empty()) {
        errOutput = Errors::missingDefinition("output_type").getAll();
    } else if (project.language.empty()) {
        errOutput = Errors::missingDefinition("language").getAll();
        return false;
    } else if (project.name.empty()) {
        errOutput = Errors::missingDefinition("name").getAll();
        return false;
    } else if (project.python_interpreter.empty()) {
        errOutput = Errors::missingDefinition("python_interpreter").getAll();
        return false;
    } else if (project.sources.empty() && !project.nothingMoreToGen) {
        errOutput = Errors::missingDefinition("sources").getAll();
        return false;
    }

    _rawPath = project.rawPath;

    std::string lang = project.language, comp, raw, type = project.output_type, staticLib, sharedLib;
    std::ostringstream objects;

	dir = JoystickDirectory(_rawPath, project.name);
	dir.update();

    int last;

    if (lang == "C" || lang == "c") {
        comp = (comp.empty()) ? project.c_compiler : "";
    } else if (lang == "C++" || lang == "c++" || lang == "cpp") {
        comp = (comp.empty()) ? project.cpp_compiler : "";
    } else if (lang == "Python" || lang == "python" || lang == "py") {
        // TODO: Implement python functionality
        comp = (comp.empty()) ? project.python_interpreter : "";
    } else {
        errOutput = Errors::unknownLanguage(lang).getAll();
        return false;
    }

    if (type == "static library" || type == "Static Library") {
        staticLib = "ar rs";
    } else if (type == "dynamic library" || type == "Dynamic Library"
               || type == "shared library" || type == "Shared Library") {
        sharedLib = "-shared";
        project.output_name = "lib" + project.output_name + ".so";
    } else if (type == "binary" || type == "executable" || type == "Binary" || type == "Executable") {
        // Executable
    } else {
        errOutput = Errors::unknownType(type).getAll();
        return false;
    }

    for (std::string source : project.sources) {
        command.str("");

        last = source.find_last_of('.');
        raw = source.substr(0, last);

        if (raw.find("/") != std::string::npos) {
            last = raw.find_last_of('/');
            raw = raw.substr(last, raw.size());
        }

        std::string objectFile = _rawPath + "/.joystick/objects/" + project.name + "/" + raw + ".o";

        if (lang == "C++" || lang == "C" || lang == "c++" || lang == "c" || lang == "cpp") {
            command << comp
                    << " -c " + _rawPath + '/' + source
                    << " -o " + objectFile;
        } else {
            command << project.python_interpreter;
        }

        if (!sharedLib.empty())
            command << " -fPIC ";

        for (std::string includePath : project.include_paths)
            command << " -I" << includePath;

        if (!project.extra_compilation_options.empty())
            command << ' ' << project.extra_compilation_options;

        Log::buildLogBold("Compiling " + source + "...", CONSOLE_COLOR_CYAN);

        multithreadComp();

        Log::buildLog(compOut, CONSOLE_COLOR_WHITE);
        Log::buildLog(warnings, CONSOLE_COLOR_WHITE);

        if (!errOutputCommand.empty()) {
            // Error occured
            errOutput = "Compilation failed";
            return false;
        }

        errOutputStream.close();

    }

    command.str("");

    for (auto const& file : std::filesystem::directory_iterator(_rawPath + "/.joystick/objects/" + project.name + '/'))
        if (!std::filesystem::is_directory(file.path().string())) objects << file.path().string() << " ";

    if (!staticLib.empty())
        command << staticLib << " " << project.output_name << " " << objects.str();
    else if (!sharedLib.empty())
        command << comp << " " << sharedLib << " -o " << project.output_name << ' ' << objects.str() + " -Wall";
    else
        command << comp << " " << objects.str() << " -o " << project.output_name + " -Wall";

   if (staticLib.empty() && sharedLib.empty()) {
        for (std::string sharedLibLink : project.shared_lib)
            command << " -l" << sharedLibLink;

        if (!project.extra_compilation_options.empty())
            command << ' ' << project.extra_compilation_options;

        for (std::string includePath : project.include_paths)
            command << " -I" << includePath;

        for (std::string linkPath : project.link_paths)
            command << " -L" << linkPath;

        if (!project.framework.empty() && project.cpp_compiler == "clang++")
            command << " -framework " << project.framework;
    }
    
    Log::buildLogBold("Finishing " + project.output_name + "...", CONSOLE_COLOR_BLUE);

    multithreadComp(staticLib.empty() && sharedLib.empty());

    Log::buildLog(compOut, CONSOLE_COLOR_WHITE);
    Log::buildLog(warnings, CONSOLE_COLOR_WHITE);

    errOutputStream.close();

    if (!errOutputCommand.empty()) {
        // Error occured
        errOutput = "Compilation failed";
        Log::buildLog(errOutputCommand, CONSOLE_COLOR_WHITE);
        return false;
    }

    lastCompileTimeStream.open(_rawPath + "/.joystick/objects/" + project.name + "/dat/.last_comp_time");
    lastCompileTimeStream << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    lastCompileTimeStream.close();

    return true;
}
