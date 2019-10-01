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