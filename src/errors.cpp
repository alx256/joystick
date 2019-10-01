#include "errors.h"

std::string Error::getContents() { return contents; }
std::string Error::getIdentifier() { return identifier; }
std::string Error::getName() { return name; }
std::string Error::getAll() { return "[" + name + "] " + contents + " (" + identifier + ")\n"; }

Error Errors::invalidParameter(const std::string& parameter, int lineNum) {
        return Error("Invalid parameter " + parameter + " on line " + std::to_string(lineNum),
        "Totem Parameter Error", "A");
}

Error Errors::unknownFunction(const std::string& function, int lineNum) {
        return Error("Unknown function " + function + " on line " + std::to_string(lineNum),
        "Totem Function Error", "B");
}

Error Errors::unknownInstruction(const std::string& instruction, int lineNum) {
        return Error("Unknown instruction " + instruction + " on line " + std::to_string(lineNum),
        "Totem Instruction Error", "C");
}

Error Errors::unknownLanguage(const std::string& language) {
        return Error("Unknown language " + language, "Unknown Language Error", "F1");
}

Error Errors::unknownType(const std::string& type) {
        return Error("Unknown type " + type, "Unknown Type Error", "F2");
}

Error Errors::missingEndSymbol(const std::string& type /* Project or Function*/, 
const std::string& name /* The name of the project/function*/) {
        if (type == "project") {
                return Error("Missing end symbol for project block", "Totem Missing Symbol Error", "D1");
        }
        return Error("Missing end symbol for " + type + " " + name,
        "Totem Missing Symbol Error", "D2");
}

Error Errors::missingDefinition(const std::string& def) {
        return Error("Missing project definition for " + def, 
        "Missing Definition Error", "E");
}
