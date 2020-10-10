# Rubik-Cube

OpenGL/C++ exercise to render a Rubik's Cube and solve a scrambled version.

Supports cube sizes from 1 to 200. Change this value on line 9 of src/config.h. Larger cube sizes demand more computer resources and take longer to solve.

Use the left mouse button to click and drag the cube around its center. Use W to zoom in, S to zoom out. Press V to readjust the camera to default settings. Press ENTER between stages of solving to proceed to the next one. Press ESCAPE to exit.

Main idea is to first render a hollow cube, to save resources, with the appriopriate colors. Upon one rotation of a face, update the position of the vertices of the appropriate cubies on the fly in the vertex shader (not in main). Once that one particular rotation is finished, use the same vertices in main and update them to update transformation / colors appropriately (otherwise the colors are as if no rotation happened at all). For solving, we first solve the centers, then solve the edges, then solve the rest as a large 3x3x3 cube. 

Requirements for Ubuntu:\
`apt-get install cmake`\
`apt-get install libglfw3-dev`\
`apt-get install libglew-dev`\
`apt-get install libpng-dev`\
`apt-get install libjpeg-dev`

To run in Ubuntu, 
1. `mkdir build`

to build the project for the first time, and then to run the actual program,

2. `./run.sh`

I am aware that many such virtual Rubik's Cube solvers exist elsewhere. Inspiration for this project is from https://www.youtube.com/watch?v=f9smvQ5fc7Q and https://www.youtube.com/watch?v=IM2czqavlWM by Code Bullet. The general outline followed is the reduction method, given by https://ruwix.com/twisty-puzzles/big-cubes-nxnxn-solution/. 

Starter code is fully attributed to Professor Etienne Vouga, used from previous projects from Graphics class. 
