#!/bin/bash

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
