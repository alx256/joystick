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

#include "runcommand.h"

std::string runCommand(const std::string& command, bool debug) {
    std::array<char, 128> buffer;
    std::string ret;
    std::ostringstream ss;

    if (debug) {
        ss << "DEBUG: (Thread " << std::this_thread::get_id() << ") " << command << '\n';
        ret += ss.str();
    }

    FILE* pipe = popen(command.c_str(), "r");
    while (fgets(buffer.data(), 128, pipe) != NULL)
        ret += buffer.data();

    pclose(pipe);

    return ret;
}
