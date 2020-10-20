#!/bin/sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8 
cd ..

if [ -z "$1" ] 	# if argument 1 does NOT exists
then
	./build/bin/animation
else			# has user args
	./build/bin/animation "$1"
fi
