#!/bin/bash

#  Copyright 2020 Alexander Poulson
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

J_BOOST_LINK=/usr/local/include/
J_ARGS="-pthread -std=c++17 -I/usr/local/include"
J_FAILED=false
J_OBJS=""

if  [[ "$OSTYPE" == "darwin"* ]]; then
	J_COMP="/Library/Developer/CommandLineTools/usr/bin/c++"
else
	J_COMP="c++"
fi

if [[ "$1" == "--with-ncursesw" ]]; then
	J_ARGS+=" -DUSE_NCURSESW=yes"
fi

echo "Building Joystick..."

cd src

echo ""

for file in *.cpp; do
	echo "Building $file"
	${J_COMP} ${J_ARGS} -c $file -o ${file}.o
	if [ $? -ne 0 ]; then
		echo "Building Joystick failed!"
		J_FAILED=true
		break
	fi
	echo "Done"
	J_OBJS+=" $file.o"
done

if [ $J_FAILED = false ]; then
	${J_COMP} -pthread ${J_ARGS} ${J_OBJS} -o joystick -lcurses -lform -lboost_system -lboost_filesystem -L/usr/local/lib
	if [ $? -eq 0 ]; then
		echo "Building Joystick complete!"
		mv joystick ..
		rm ${J_OBJS}
	fi
fi
