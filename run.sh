#!/bin/sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j8 
cd ..
./build/bin/animation ./assets/pmd/Haku_Yowane.pmd
