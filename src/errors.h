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

#include <string>

class Error {
public:
    Error(std::string _contents, std::string _name, std::string _identifier):
        contents(_contents), name(_name), identifier(_identifier) {}

    std::string getContents();
    std::string getName();
    std::string getIdentifier();
    std::string getAll();

private:
    std::string contents, name, identifier;

};

struct Errors {
    static Error invalidParameter(const std::string& parameter, int lineNum);
    static Error unknownFunction(const std::string& function, int lineNum);
    static Error unknownInstruction(const std::string& instruction, int lineNum);
    static Error unknownLanguage(const std::string& language);
    static Error unknownType(const std::string& type);
    static Error missingEndSymbol(const std::string& type, const std::string& name = "");
    static Error missingDefinition(const std::string& def);
};
