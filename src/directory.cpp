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

#include "directory.h"

JoystickDirectory::JoystickDirectory(const std::string& rawPath, const std::string& projName): 
	prefix(rawPath + "/.joystick/"), status(false) {
	directories = {
		"objects",
		"objects/" + projName,
		"objects/" + projName + "/dat",
		"output"
	};
	files = {
		"objects/" + projName + ".last_comp_time",
		"output/.comp_out"
	};
}

bool JoystickDirectory::update() {
    updateRequired = !boost::filesystem::exists(prefix);

    for (std::string directory : directories)
        updateRequired |= !boost::filesystem::exists(prefix + directory);

    if (updateRequired) {
		boost::filesystem::create_directory(prefix);

		for (const std::string& dir : directories) {
			status |= boost::filesystem::create_directory(prefix + dir);
		}

		for (const std::string& file : files) {
			std::system(std::string("touch " + prefix + file).c_str());
		}
    }

	return status;
}
