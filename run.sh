#!/bin/sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8 
cd ..
./build/bin/animation example0003A.json
