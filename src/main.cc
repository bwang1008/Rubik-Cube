#include <GL/glew.h>

#include "procedure_geometry.h"		// create_rubik, update_rubik
#include "render_pass.h"			// helper for rendering with OpenGL
#include "config.h"					// Constants
#include "gui.h"					// GUI for OpenGL
#include "cube.h"					// create_cube for each individual small cubie
#include "solver.h"					// Solver object -> algorithms to solve scrambled Rubik's cube

#include <memory>					
#include <algorithm>				
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <set>						// keep track of which layers have been turning; prevent (2,4,2),(2,6,2),(2,4,2) from executing all at once

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <debuggl.h>

#include <jpegio.h>
#include <cmath>
#include <stdlib.h> // rand

int window_width = 900; //1280;
int window_height = 900;

const std::string window_title = "Rubik's Cube";

const char* vertex_shader =
#include "shaders/default.vert"
;

const char* geometry_shader =
#include "shaders/default.geom"
;

const char* fragment_shader =
#include "shaders/default.frag"
;

const char* cube_vertex_shader = 
#include "shaders/cube.vert"
;

const char* cube_fragment_shader = 
#include "shaders/cube.frag"
;

const char* preview_vertex_shader =
#include "shaders/preview.vert"
;

const char* preview_fragment_shader =
#include "shaders/preview.frag"
;

void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw() {
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Disable resizing, for simplicity
	glfwWindowHint(GLFW_SAMPLES, 4);
	auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);
	CHECK_SUCCESS(ret != nullptr);
	glfwMakeContextCurrent(ret);
	glewExperimental = GL_TRUE;
	CHECK_SUCCESS(glewInit() == GLEW_OK);
	glGetError();  // clear GLEW's error for it
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported:" << version << "\n";

	return ret;
}

int main(int argc, char* argv[]) {	
	// reject invalid N
	if (cubeWidth <= 0 || cubeWidth > 200) {
		std::cerr << "Cube width defined in config.h is invalid" << std::endl;
		std::cerr << "Width cannot be negative, and large values consume too much resources" << std::endl;
		return -1;
	}

	std::cout << "Cube Width = " << cubeWidth << std::endl;

	// set up graphical window
	GLFWwindow *window = init_glefw();
	GUI gui(window, window_width, window_height, window_height);

	// tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Solver* solver = new Solver(); // create solver
	gui.loadSolver(solver); // hand it to gui
	gui.setDeque(); // hand to the solver: the deque from gui; so gui has access to deque

	// Rotation matrices;
	glm::mat4 matX1 = glm::mat4(1.0f);
	glm::mat4 matY1 = glm::mat4(1.0f);
	glm::mat4 matZ1 = glm::mat4(1.0f);

	glm::mat4 matX2 = glm::mat4(1.0f);
	glm::mat4 matY2 = glm::mat4(1.0f);
	glm::mat4 matZ2 = glm::mat4(1.0f);

	glm::mat4 matX3 = glm::mat4(1.0f);
	glm::mat4 matY3 = glm::mat4(1.0f);
	glm::mat4 matZ3 = glm::mat4(1.0f);

	// edit entries to make proper rotation matrices; curly braces so I can minimize it in text editor
	{
		// sin(pi/2) = 1 to make counter clockwise; so negate for clockwise
		matX1[1][1] = 0.0f;
		matX1[2][1] = 1.0f;
		matX1[1][2] = -1.0f;
		matX1[2][2] = 0.0f;

		matY1[0][0] = 0.0f;
		matY1[2][0] = -1.0f;
		matY1[0][2] = 1.0f;
		matY1[2][2] = 0.0f;

		matZ1[0][0] = 0.0f;
		matZ1[1][0] = 1.0f;
		matZ1[0][1] = -1.0f;
		matZ1[1][1] = 0.0f;

		matX2[1][1] = -1.0f;
		matX2[2][1] = 0.0f;
		matX2[1][2] = 0.0f;
		matX2[2][2] = -1.0f;

		matY2[0][0] = -1.0f;
		matY2[2][0] = 0.0f;
		matY2[0][2] = 0.0f;
		matY2[2][2] = -1.0f;

		matZ2[0][0] = -1.0f;
		matZ2[1][0] = 0.0f;
		matZ2[0][1] = 0.0f;
		matZ2[1][1] = -1.0f;

		matX3[1][1] = 0.0f;
		matX3[2][1] = -1.0f;
		matX3[1][2] = 1.0f;
		matX3[2][2] = 0.0f;

		matY3[0][0] = 0.0f;
		matY3[2][0] = 1.0f;
		matY3[0][2] = -1.0f;
		matY3[2][2] = 0.0f;

		matZ3[0][0] = 0.0f;
		matZ3[1][0] = -1.0f;
		matZ3[0][1] = 1.0f;
		matZ3[1][1] = 0.0f;
	}

	// Information for cubes
	std::vector<glm::vec4> cube_vertices; // list of all vertices of cubes in world coord
	std::vector<glm::uvec3> cube_faces; // all triangular faces making up all cubes
	std::vector<int> cube_types; // specify which faces of cube to actually color
	std::vector<glm::vec3> cube_centers; // centers of each cube

	// Pass containers into method to populate the vectors appropriately 
	create_rubik(cube_vertices, cube_faces, cube_types, cube_centers);

	std::vector<int> cube_rotating; // which cubes should be rotating with gui.getCurrentMove()
	for(size_t i = 0; i < cube_vertices.size(); ++i) {
		cube_rotating.push_back(0);
	}

	std::cout << "Num cubes = " << cube_vertices.size() / 8 << std::endl;
	std::cout << "Num vertices = " << cube_vertices.size() << std::endl;
	std::cout << "Num faces  = " << cube_faces.size() << std::endl;

	//glm::vec4 light_position = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);

	MatrixPointers mats; // Define MatrixPointers here for lambda to capture
	/*
	 * In the following we are going to define several lambda functions as
	 * the data source of GLSL uniforms
	 *
	 * Introduction about lambda functions:
	 *      http://en.cppreference.com/w/cpp/language/lambda
	 *      http://www.stroustrup.com/C++11FAQ.html#lambda
	 *
	 * Note: lambda expressions cannot be converted to std::function directly
	 *       Hence we need to declare the data function explicitly.
	 *
	 * CAVEAT: DO NOT RETURN const T&, which compiles but causes
	 *         segfaults.
	 *
	 * Do not worry about the efficient issue, copy elision in C++ 17 will
	 * minimize the performance impact.
	 *
	 * More details about copy elision:
	 *      https://en.cppreference.com/w/cpp/language/copy_elision
	 */

	std::function<glm::mat4()> view_data = [&mats]() { return *mats.view; };
	std::function<glm::mat4()> proj_data = [&mats]() { return *mats.projection; };
	std::function<glm::mat4()> identity_mat = [](){ return glm::mat4(1.0f); };
	std::function<glm::vec3()> cam_data = [&gui](){ return gui.getCamera(); };
	//std::function<glm::vec4()> lp_data = [&light_position]() { return light_position; };
	std::function<int()> face_data = [&gui]() { return gui.getCurrentMove()[0]; };
	std::function<float()> theta_data = [&gui]() {
		float temp = -gui.getCurrentPlayTime() * gui.getRotatingSpeed();
		if (gui.getCurrentMove()[2] < 0)
			temp = -temp;
		return temp;
	};

	auto std_view = make_uniform("view", view_data);
	auto std_camera = make_uniform("camera_position", cam_data);
	auto std_proj = make_uniform("projection", proj_data);
	//auto std_light = make_uniform("light_position", lp_data);
	auto face_uni = make_uniform("face", face_data);
	auto theta_uni = make_uniform("theta", theta_data);

	RenderDataInput cube_pass_input;
	cube_pass_input.assign(0, "vertex_position", cube_vertices.data(), cube_vertices.size(), 4, GL_FLOAT);
	cube_pass_input.assign(1, "cube_type", cube_types.data(), cube_types.size(), 1, GL_INT);
	cube_pass_input.assign(2, "rotating", cube_rotating.data(), cube_rotating.size(), 1, GL_INT);
	cube_pass_input.assignIndex(cube_faces.data(), cube_faces.size(), 3);
	RenderPass cube_pass(-1, cube_pass_input,
			{ cube_vertex_shader, nullptr, cube_fragment_shader},
			{ std_view, std_proj, face_uni, theta_uni},
			{ "fragment_color"}
			);

	int dequeSize = 0;			// to be used for progress bar display
	long long totalMoves = 0;	// meta data for counting how many moves taken to solve the entire cube
	long long totalQT = 0;
	bool draw_cube = true;		// not sure why this would ever be false
	bool finished = false;		// true only when cube is completely solved

	if(argc == 3){				// this should never be used right now, but hopefully can take in user input file
		std::string s(argv[2]);
		std::cout << "loading file: " << s << std::endl;
		//mesh.loadAnimationFrom(s);
	}
	/*
	if (argc >= 3) {
		mesh.loadAnimationFrom(argv[2]);
	}
	*/

	// SCRAMBLE
	std::cout << "Begin generating scrambles" << std::endl;
	solver -> scrambleCube();
	dequeSize = gui.getSize();
	std::cout << "Going to scramble by " << gui.getSize() << " moves" << std::endl;
	gui.setRotatingSpeed(100.0f);

	// render loop
	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);			// background color
		//glClearColor(0.56f, 0.72f, 0.95f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		gui.updateMatrices();
		mats = gui.getMatrixPointers();

		// title of window
		std::stringstream title;
		float cur_time = gui.getCurrentPlayTime();
		title << window_title;
		if (gui.isPlaying()) {
			title << " Playing: "
			      << std::setprecision(2)
			      << std::setfill('0') << std::setw(6)
			      << cur_time << " sec";
			
			//mesh.updateAnimation(cur_time);
		} 
		else {
			title << " Paused: "
				<< std::setprecision(2)
				<< std::setfill('0') << std::setw(6)
				<< cur_time << " sec";
		}

		glfwSetWindowTitle(window, title.str().data());
		glViewport(0, 0, window_width, window_height);

		// progress bar
		if (true && !finished) {
			std::cout << "Progress: [";
			int currProgress = std::round(50 * (float(dequeSize - gui.getSize()) / dequeSize));
			for (int pr = 0; pr < currProgress; ++pr) {
				std::cout << "I";
			}
			for (int pr = 0; pr < 50 - currProgress; ++pr) {
				std::cout << " ";
			}
			std::cout << "]\r";
			if (solver->currentState() % 2 == 0 && gui.getSize() == 0) {
				std::cout << std::endl;
			}
			if (solver->currentState() % 2 == 0) {
				std::cout << std::flush;
			}
		}

		// Finished scrambling
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 0) {
			//solver->copyConfiguration(cube_centers, cube_types);
			gui.resetCount();

			solver->preliminary0();
			solver->solveCenter0();
			solver->preliminary1();
			dequeSize = gui.getSize();
			gui.setRotatingSpeed(5.0f); // to solve 1st center
			
			std::cout << "Click on animation window and press ENTER to proceed" << std::endl;
			solver->incr(); // state == 1
		}

		// Finished solving first center
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 2) {
			std::cout << "num moves = " << gui.getCountMoves() << std::endl;
			std::cout << "num quarter turns = " << gui.getCountQT() << std::endl;

			totalMoves += gui.getCountMoves();
			totalQT += gui.getCountQT();
			
			gui.resetCount();
			solver->solveCenter1();
			dequeSize = gui.getSize();
			gui.setRotatingSpeed(5.0f); // to solve 2nd center

			std::cout << "Click on animation window and press ENTER to proceed (2)" << std::endl;
			solver->incr();
		}

		// Finished solving second center
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 4) {
			std::cout << "num moves = " << gui.getCountMoves() << std::endl;
			std::cout << "num quarter turns = " << gui.getCountQT() << std::endl;

			totalMoves += gui.getCountMoves();
			totalQT += gui.getCountQT();

			gui.resetCount();
			solver->solveCenter2();
			dequeSize = gui.getSize();
			gui.setRotatingSpeed(5.0f); // to solve 3rd center

			std::cout << "Click on animation window and press ENTER to proceed (3)" << std::endl;
			solver->incr();
		}

		// Finished solving third center
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 6) {
			std::cout << "num moves = " << gui.getCountMoves() << std::endl;
			std::cout << "num quarter turns = " << gui.getCountQT() << std::endl;

			totalMoves += gui.getCountMoves();
			totalQT += gui.getCountQT();

			gui.resetCount();
			solver->solveCenter3();
			dequeSize = gui.getSize();
			gui.setRotatingSpeed(5.0f); // to solve 4th center

			std::cout << "Click on animation window and press ENTER to proceed (4)" << std::endl;
			solver->incr();
		}

		// Finished solving fourth center
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 8) {
			std::cout << "num moves = " << gui.getCountMoves() << std::endl;
			std::cout << "num quarter turns = " << gui.getCountQT() << std::endl;

			totalMoves += gui.getCountMoves();
			totalQT += gui.getCountQT();

			gui.resetCount();
			solver->solveLastCenters();
			dequeSize = gui.getSize();
			gui.setRotatingSpeed(5.0f); // to solve 5th, 6th centers

			std::cout << "Click on animation window and press ENTER to proceed (5)" << std::endl;
			solver->incr();
		}

		// Finished solving all centers
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 10) {
			std::cout << "num moves = " << gui.getCountMoves() << std::endl;
			std::cout << "num quarter turns = " << gui.getCountQT() << std::endl;

			totalMoves += gui.getCountMoves();
			totalQT += gui.getCountQT();

			std::cout << "total moves of centers = (" << totalMoves << ", " << totalQT << ")" << std::endl;

			gui.resetCount();
			solver->solveEdges();
			dequeSize = gui.getSize();
			gui.setRotatingSpeed(5.0f); // to solve edges

			std::cout << "Click on animation window and press ENTER to proceed (6)" << std::endl;
			solver->incr();
		}

		// Finished solving all edges
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 12) {
			std::cout << "num moves = " << gui.getCountMoves() << std::endl;
			std::cout << "num quarter turns = " << gui.getCountQT() << std::endl;

			totalMoves += gui.getCountMoves();
			totalQT += gui.getCountQT();

			//std::cout << "total moves = (" << totalMoves << ", " << totalQT << ")" << std::endl;

			gui.resetCount();
			solver->solve3x3x3();
			dequeSize = gui.getSize();
			gui.setRotatingSpeed(5.0f); // to solve 3x3x3

			std::cout << "Click on animation window and press ENTER to proceed (7)" << std::endl;
			solver->incr();
		}

		// FINISHED
		if (gui.getSize() == 0 && gui.getCurrentMove()[0] < 0 && solver->currentState() == 14) {
			std::cout << "num moves = " << gui.getCountMoves() << std::endl;
			std::cout << "num quarter turns = " << gui.getCountQT() << std::endl;

			totalMoves += gui.getCountMoves();
			totalQT += gui.getCountQT();

			std::cout << "total moves = (" << totalMoves << ", " << totalQT << ")" << std::endl;
			gui.resetCount();
			gui.setRotatingSpeed(5.0f);

			std::cout << "SOLVED" << std::endl;
			solver->incr();
			finished = true;
		}
	
		// update rotation, depending if rotating already or not
		if(gui.isQuarterTurning()) {
			float currentTheta = std::abs(gui.getCurrentPlayTime() * gui.getRotatingSpeed());
			glm::ivec3 myMove = gui.getCurrentMove();
			int face = myMove[0];
			int turns = ((myMove[2] % 4) + 4) % 4;
			float supposedRadians = float((std::abs(myMove[2])) * 3.1415926/2.0);

			// if we turned enough, stop and update for next turn
			if(currentTheta > supposedRadians && face >= 0) { // if it turned enough, stop turning
				// update cube vertices (VBO) and centers after making quarter turns
				// to make transformations permanent
				glm::mat4 mat = glm::mat4(1.0f);

				if(face == 0) { // front
					if(turns == 1) {
						mat = matZ1;
					}
					else if(turns == 2) {
						mat = matZ2;
					}
					else if(turns == 3) {
						mat = matZ3;
					}
				}
				else if(face == 1) { // right
					if(turns == 1) {
						mat = matX1;
					}
					else if(turns == 2) {
						mat = matX2;
					}
					else if(turns == 3) {
						mat = matX3;
					}
				}
				else if(face == 2) { // top
					if(turns == 1) {
						mat = matY1;
					}
					else if(turns == 2) {
						mat = matY2;
					}
					else if(turns == 3) {
						mat = matY3;
					}
				}
				else if(face == 3) { // bottom
					if(turns == 1) {
						mat = matY3;
					}
					else if(turns == 2) {
						mat = matY2;
					}
					else if(turns == 3) {
						mat = matY1;
					}
				}
				else if(face == 4) { // left
					if(turns == 1) {
						mat = matX3;
					}
					else if(turns == 2) {
						mat = matX2;
					}
					else if(turns == 3) {
						mat = matX1;
					}
				}
				else if(face == 5) { // back
					if(turns == 1) {
						mat = matZ3;
					}
					else if(turns == 2) {
						mat = matZ2;
					}
					else if(turns == 3) {
						mat = matZ1;
					}
				}

				for(size_t i = 0; i < cube_centers.size(); i += 8) {
					if(cube_rotating[i] == 1) { // if it was rotating
						// update final positions of vertices (and cubie center)
						glm::vec4 origPos = glm::vec4(cube_centers[i], 1.0f);
						glm::vec4 newPos = mat * origPos;

						for(int j = 0; j < 8; ++j) {
							cube_centers[i + j] = newPos;
						}

						cube_vertices[i + 0] = glm::vec4(newPos[0] - 0.5, newPos[1] - 0.5, newPos[2] - 0.5, 1);
						cube_vertices[i + 1] = glm::vec4(newPos[0] + 0.5, newPos[1] - 0.5, newPos[2] - 0.5, 1);
						cube_vertices[i + 2] = glm::vec4(newPos[0] - 0.5, newPos[1] + 0.5, newPos[2] - 0.5, 1);
						cube_vertices[i + 3] = glm::vec4(newPos[0] + 0.5, newPos[1] + 0.5, newPos[2] - 0.5, 1);
						cube_vertices[i + 4] = glm::vec4(newPos[0] - 0.5, newPos[1] - 0.5, newPos[2] + 0.5, 1);
						cube_vertices[i + 5] = glm::vec4(newPos[0] + 0.5, newPos[1] - 0.5, newPos[2] + 0.5, 1);
						cube_vertices[i + 6] = glm::vec4(newPos[0] - 0.5, newPos[1] + 0.5, newPos[2] + 0.5, 1);
						cube_vertices[i + 7] = glm::vec4(newPos[0] + 0.5, newPos[1] + 0.5, newPos[2] + 0.5, 1);
						
						
						// now update axes of cube (absolute orientation)
						int myType = (cube_types[i] >> 6) & 4095; // get first 12 bits
						
						// front, right, top axes, each 4 bits
						int frontBits = (myType >> 8) & 15;
						int rightBits = (myType >> 4) & 15;
						int topBits = (myType) & 15;
						
						glm::vec4 frontAxis = glm::vec4((frontBits >> 2) & 1, (frontBits >> 1) & 1, frontBits & 1, 0.0f);
						glm::vec4 rightAxis = glm::vec4((rightBits >> 2) & 1, (rightBits >> 1) & 1, rightBits & 1, 0.0f);
						glm::vec4 topAxis = glm::vec4((topBits >> 2) & 1, (topBits >> 1) & 1, topBits & 1, 0.0f);
						glm::vec4 lastCol = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

						// apply negative sign
						if ((frontBits >> 3) == 1) {
							frontAxis = -frontAxis;
						}
						if ((rightBits >> 3) == 1) {
							rightAxis = -rightAxis;
						}
						if ((topBits >> 3) == 1) {
							topAxis = -topAxis;
						}

						// apply final rotation
						glm::mat4 origAxes = glm::mat4(frontAxis, rightAxis, topAxis, lastCol);
						glm::mat4 newAxes = mat * origAxes;

						// get back axes
						frontAxis = newAxes[0];
						rightAxis = newAxes[1];
						topAxis = newAxes[2];

						int frontAxisNum = int(9 * frontAxis[0] + 3 * frontAxis[1] + frontAxis[2]);
						int rightAxisNum = int(9 * rightAxis[0] + 3 * rightAxis[1] + rightAxis[2]);
						int topAxisNum = int(9 * topAxis[0] + 3 * topAxis[1] + topAxis[2]);
						
						if (frontAxisNum < 0) {
							frontAxis = -frontAxis;
							frontAxisNum = int(8 + (4 * frontAxis[0] + 2 * frontAxis[1] + frontAxis[2]));
						}
						else {
							frontAxisNum = int(4 * frontAxis[0] + 2 * frontAxis[1] + frontAxis[2]);
						}
						if (rightAxisNum < 0) {
							rightAxis = -rightAxis;
							rightAxisNum = int(8 + (4 * rightAxis[0] + 2 * rightAxis[1] + rightAxis[2]));
						}
						else {
							rightAxisNum = int(4 * rightAxis[0] + 2 * rightAxis[1] + rightAxis[2]);
						}
						if (topAxisNum < 0) {
							topAxis = -topAxis;
							topAxisNum = int(8 + (4 * topAxis[0] + 2 * topAxis[1] + topAxis[2]));
						}
						else {
							topAxisNum = int(4 * topAxis[0] + 2 * topAxis[1] + topAxis[2]);
						}

						int newType = int(frontAxisNum * 256 + rightAxisNum * 16 + topAxisNum) * 64;
						newType += (cube_types[i] & 63);

						// finally update cube_types 
						for (int j = 0; j < 8; ++j) {
							cube_types[i + j] = newType;
						}
					}
				}

				// updateVBO of vertices and types
				cube_pass.updateVBO(0, cube_vertices.data(), cube_vertices.size());
				cube_pass.updateVBO(1, cube_types.data(), cube_types.size());

				gui.setQuarterTurning(false);
			}
		}
		if(!gui.isQuarterTurning()) { // currently not turning, so start next move
			glm::ivec3 nextMove = gui.peekMove();

			std::set<int> whichLayers; // prevent executing (2,4,2),(2,6,2),(2,4,2) all at once in parallel

			bool firstTime = true;
			while (true) { // bad coding style, but trying to make multi-layer turn work

				gui.setCurrentMove();
				glm::ivec3 myMove = gui.getCurrentMove(); // Get the next move

				//std::cout << "myMove = " << myMove[0] << " " << myMove[1] << " " << myMove[2] << std::endl;

				if (nextMove[0] >= 0) { // only rotate if valid face
					whichLayers.insert(myMove[1]);

					gui.setStartTime(); // set time at 0
					update_rubik(cube_centers, myMove, cube_rotating, firstTime); // find which cubes should be rotating

					cube_pass.updateVBO(2, cube_rotating.data(), cube_rotating.size());
					gui.setQuarterTurning(true); // Applying next move


					if (gui.peekMove()[0] == nextMove[0] && gui.peekMove()[2] == nextMove[2] && whichLayers.find(gui.peekMove()[1]) == whichLayers.end()) {
						firstTime = false;
						continue;
					}
				}

				break;
			}
		}

		// Render cubes
		if (draw_cube) {
			cube_pass.setup();
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, cube_faces.size() * 3, GL_UNSIGNED_INT, 0));
		}

		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// outside of rendering loop, do clean up
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
