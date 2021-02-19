# Rubik-Cube

OpenGL/C++ exercise to render a Rubik's Cube and solve a scrambled version.

Supports cube sizes from 1 to 200. Change this value on line 21 of src/config.h. Larger cube sizes demand more computer resources and take longer to solve. This is intended to run in Ubuntu.

Use the left mouse button to click and drag the cube around its center. Use W to zoom in, S to zoom out. Press V to re-adjust the camera to default settings. Press ENTER between stages of solving to proceed to the next one. Press ESCAPE to exit.

Main idea is to first render a hollow cube, to save resources, with the appriopriate colors. Upon one rotation of a face, update the position of the vertices of the appropriate cubies on the fly in the vertex shader (not in main). Once that one particular rotation is finished, use the same vertices in main and update them to update transformation / colors appropriately (otherwise the colors are as if no rotation happened at all). For solving, we first solve the centers, then solve the edges, then solve the rest as a large 3x3x3 cube. 

File directory structure:
|- cmake - Folder - used for CMake
|- lib   - Folder - utilities and abstractions for jpeg,image,material,...
|- src   - Folder - actual source code for project
	|- shaders 					- Folder - vertex shader and fragment shader for GLSL
	|- CMakeLists.txt 			- File - CMake
	|- config.h 				- File - Constants 
	|- cube.h 					- File - Specifies vertices and triangles to build a unit cube
	|- gui.cc 					- File - Controls for mouse and keyboard in animation
	|- gui.h 					- File - header for gui.cc
	|- json.hpp 				- File - Nlohmann's json for C++
	|- main.cc 					- File - Rendering loop
	|- procedure_geometry.cc 	- File - Specifies how to build large Rubik's cube from individual small cubes from cube.h
	|- procedure_geometry.h 	- File - header file for procedure_geometry.cc
	|- render_pass.cc 			- File - abstraction for rendering objects in OpenGL
	|- render_pass.h 			- File - header file for render_pass.cc
	|- shader_uniform.cc 		- File - abstraction for GLSL commands
	|- shader_uniform.h 		- File - header file for shader_uniform.cc
	|- solver.cc 				- File - implementation that solves Rubik's cube and generates the moves for it
	|- solver.h 				- File - header file for solver.cc
|- third-party 		- Folder - this should be for OpenGL mathematics library
|- .gitignore  		- File - what should not be committed to git repo
|- CMakeLists.txt 	- File - CMake
|- Log.txt 			- File - Explanation of what happened in the project in each git commit
|- README.md 		- File - This current file
|- TODO.txt 		- File - Ideas for what to implement next
|- run.sh 			- File - Bash script to run the program


Requirements for Ubuntu:\
`apt-get install cmake`\
`apt-get install libglfw3-dev`\
`apt-get install libglew-dev`\
`apt-get install libpng-dev`\
`apt-get install libjpeg-dev`



To run in Ubuntu:

## Clone the repository

1. `git clone https://github.com/bwang1008/Rubik-Cube.git`

2. `cd Rubik-Cube`

## Mark the bash script as executable.

3. `chmod +x run.sh` 

## Make a build directory

4. `mkdir build`

This directory is used to build the project for the first time. Only do the above steps one time each!

## Run the Program

1. Change the size of the Rubik's Cube in file `src/config.h`, line 21. The default should be `const int cubeWidth = 2;`. 

2. `./run.sh`

I am aware that many such virtual Rubik's Cube solvers exist elsewhere. Inspiration for this project is from https://www.youtube.com/watch?v=f9smvQ5fc7Q and https://www.youtube.com/watch?v=IM2czqavlWM by Code Bullet. The general outline followed is the reduction method, given by https://ruwix.com/twisty-puzzles/big-cubes-nxnxn-solution/. Starter code is fully attributed to Professor Etienne Vouga, used from previous projects from class. 
