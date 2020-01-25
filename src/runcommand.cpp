#include "runcommand.h"

std::string runCommand(const char * command, bool debug) {
    std::array<char, 128> buffer;
    std::string ret;
    std::ostringstream ss;

    if (debug) {
        ss << "DEBUG: (Thread " << std::this_thread::get_id() << ") " << command << '\n';
        ret += ss.str();
    }

    FILE* pipe = popen(command, "r");
    while (fgets(buffer.data(), 128, pipe) != NULL)
        ret += buffer.data();

    pclose(pipe);

    return ret;
}
