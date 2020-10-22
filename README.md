# Rubik-Cube

OpenGL/C++ exercise to render a Rubik's Cube and solve a scrambled version.

Supports cube sizes from 1 to 200. Change this value on line 10 of src/config.h. Larger cube sizes demand more computer resources and take longer to solve.

Use the left mouse button to click and drag the cube around its center. Use W to zoom in, S to zoom out. Press V to readjust the camera to default settings. Press ENTER between stages of solving to proceed to the next one. Press ESCAPE to exit.

The main idea is to first render a hollow cube, to save resources, with the appriopriate colors. Upon one rotation of a face, update the position of the vertices of the appropriate cubies on the fly in the vertex shader (not in main). Once that one particular rotation is finished, use the same vertices in main and update them to update transformation / colors appropriately (otherwise the colors are as if no rotation happened at all). For solving, we first solve the centers, then solve the edges, then solve the rest as a large 3x3x3 cube. 

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

To run the program with a JSON input, specify the path of the JSON file right after, like so:

3. `./rush.sh example0003A.json`

However, do not change the value of kMaxWidth (4096). Loading JSON relies on this being 4096. This is because a single move is stored into JSON as a nice 2 bytes: 2 bits for front/right/up, 2 bits for 90/180/270 degrees, and 12 bits to specify at most which of the 4096 layers should be moved. `kCubeWidth` must be an integer between 1 and `kCubeWidth` inclusive. I do not recommend anything above 1024, however.

The orientation of the faces as input inside the JSON is as follows. Suppose you view the cube with one facing directly facing you. Let this be face `front`. Let the y-axis be a straight line from your eyes to the center of `front`, pass through the center of the cube, and pass through the center of the back face of the cube. Let the x-axis be a straight line passing through the centers of the left face and right face. Let the z-axis be a straight line passing through the center of the top of the cube to the bottom face of the cube. Then

* the `front` face is the face that directly faces you
* the `right` face is the face that you see if you start from `front`, then rotate the cube along the z-axis by 90 degrees clockwise (viewed from the top)
* the `up` face is the face that you see if you start from `front`, then rotate the cube along the x-axis by 90 degrees counter-clockwise (when viewed from the right side)
* the `down` face is the face that you see if you start from `front`, then rotate the cube along the x-axis by 90 degrees clockwise (when viewed from the right side)
* the `left` face is the face that you see if you start from `front`, then rotate the cube along the z-axis by 90 degrees counter-clockwise (viewed from the top)
* the `back` face is the face that you see if you start from `front`, then rotate the cube along the z-axis 180 degrees.

## JSON Input
* `N`: You must specify a N value; you must change 'const int kCubeWidth' value in config.h to match this value.

* `colors`: To specify what colors your Rubik's cube stickers are, specify a list of three ints for each symbol you use for the sticker (optional)

* 'front' / 'right' / 'up' / 'down' / 'left' / 'back': Each must have `N` strings, each of length `N`, separated by commas. You can use any 6 distinct ASCII symbol to represent each sticker, but you must be consistent. The orientation must be the same as specified above. 


I am aware that many such virtual Rubik's Cube solvers exist elsewhere. Inspiration for this project is from https://www.youtube.com/watch?v=f9smvQ5fc7Q and https://www.youtube.com/watch?v=IM2czqavlWM by Code Bullet. The general outline followed is the reduction method, given by https://ruwix.com/twisty-puzzles/big-cubes-nxnxn-solution/. 

Starter code for setting up OpenGL is fully attributed to Professor Etienne Vouga, used from previous projects from Graphics class.
