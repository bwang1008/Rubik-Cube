#include <GL/glew.h> 					// libglew-dev
					
#include "config.h"						// global constants					
#include "gui.h"						// gui handles keyboard/mouse
#include "procedure_geometry.h"			// generate all 3D objects
#include "render_pass.h"				// helper for rendering in OpenGL
#include "solver.h"						// Solver object -> algorithms to solve scrambled Rubik's Cube

#include <iostream>						// std::cout, std::endl

int window_width =  600;
int window_height = 600;

const std::string window_title = "Rubik's Cube";

// Load shaders
const char* cube_vertex_shader = 
#include "shaders/cube.frag"
;

const char* cube_fragment_shader = 
#include "shaders/cube.vert"
;

void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

// create a window that displays the graphics
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

	const int N = kCubeWidth;

	if(N <= 0 || N >= kMaxWidth) {									// check cubeWidth is valid
		std::cerr << "Cube width defined in config.h is invalid" << std::endl;
		std::cerr << "Width cannot be negative, and large values consume too much resources" << std::endl;
		return -1;
	}

	GLFWwindow* window = init_glefw();								// setup window that displays graphics
	GUI gui(window, window_width, window_height, window_height); 	// have window interact with keyboard/mouse

	




	return 0;
}