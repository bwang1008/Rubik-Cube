#include <GL/glew.h> 					// libglew-dev
					
#include "config.h"						// global constants					
#include "gui.h"						// gui handles keyboard/mouse
#include "handle_json.h"				// read & write json files
#include "procedure_geometry.h"			// generate all 3D objects
#include "render_pass.h"				// helper for rendering in OpenGL
#include "solver.h"						// Solver object -> algorithms to solve scrambled Rubik's Cube

#include <iostream>						// std::cout, std::endl, std::cerr
#include <vector>						// std::vector

#define N kCubeWidth					// just because N is shorter to type

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

void create_textures2(Image* images[6], Solver* solver, std::vector<glm::uvec3> rgbs) {
	// assumes solver.faces is already filled
	
	int append = (4 - ((3 * kCubeWidth) % 4)) % 4;	// Weird alignment with Image: needs number of bytes on width, to be divisible by 4.

	for(int f = 0; f < 6; ++f) {					// for each of the six faces,
		Image* im = new Image();					// create a base texture / image

		im->width  = kCubeWidth;					// fill in width and height
		im->height = kCubeWidth;

		for(int i = 0; i < kCubeWidth; ++i) {
			for(int j = 0; j < kCubeWidth; ++j) {	// fill in each row and column of texture / image
				int color = solver->get(f, i, j);	// grab the value given from Solver.faces[6][N][N]
				
				im->bytes.push_back(rgbs[color][0]);	
				im->bytes.push_back(rgbs[color][1]);
				im->bytes.push_back(rgbs[color][2]);	// append each R,G,B value of the color
			}
			for(int j = 0; j < append; ++j) {		// append padding so alignment divisible by 4
				im->bytes.push_back(0);
			}
		}

		images[f] = im;								// Assign created Image into parameter images
	}
}

int main(int argc, char* argv[]) {

	if(N <= 0 || N >= kMaxWidth) {									// check cubeWidth is valid
		std::cerr << "Cube width defined in config.h is invalid" << std::endl;
		std::cerr << "Width cannot be negative, and large values consume too much resources" << std::endl;
		return -1;
	}

	std::cout << "N = " << N << std::endl;

	GLFWwindow* window = init_glefw();								// setup window that displays graphics
	GUI gui(window, window_width, window_height, window_height); 	// have window interact with keyboard/mouse

	std::vector<glm::vec4> cube_vertices;
	std::vector<glm::uvec3> cube_faces;

	create_large_cube(cube_vertices, cube_faces);					// procedure_geometry::create_large_cube to fill in vertices and faces of overall rubik's cube

	Image* images[6];												// 6 different textures for each face of cube
	Solver* solver = new Solver();									// Solver container holds colors of all 6 faces
	std::vector<glm::uvec3> rgbs;									// map from int (0-5) to rgb

	// user arguments
	if(argc >= 2) {
		std::string path(argv[1]);										
		int res = read_json(path, solver, rgbs);								// handle_json::read_json
		if(res < 0) {
			return -1;
		}
	}
	else {
		rgbs.push_back(glm::uvec3(46, 139, 87));
		rgbs.push_back(glm::uvec3(255, 0, 0));
		rgbs.push_back(glm::uvec3(255, 255, 255));
		rgbs.push_back(glm::uvec3(255, 255, 0));
		rgbs.push_back(glm::uvec3(255, 140, 0));
		rgbs.push_back(glm::uvec3(0, 0, 255));
	}
	// end user arguments

	create_textures2(images, solver, rgbs);							// procedure_geoemetry::create_textures to fill in images with correct bytes
	
	/*
	unsigned int textureNum[6];										// generated number for each texture (?)
	for (int i = 0; i < 6; ++i) {									
		glGenTextures(1, &(textureNum[i]));							// create actual number
		glBindTexture(GL_TEXTURE_2D, textureNum[i]);						
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Snap to nearest pixel
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, images[i]->width, images[i]->height, 0, GL_RGB, GL_UNSIGNED_BYTE, images[i]->bytes.data());	// generate actual texture
		glGenerateMipmap(GL_TEXTURE_2D);							// mipmaps for when zooming in or out

		std::cout << "Generated image " << (i+1) << std::endl;
	}
	*/
	std::cout << "Generated all 6 images" << std::endl;

	MatrixPointers mats;												// Define MatrixPointers here for lambda to capture

	// lambdas for uniforms
	std::function<glm::mat4()> view_data = [&mats]() { return *mats.view; };
	std::function<glm::mat4()> proj_data = [&mats]() { return *mats.projection; };
	std::function<glm::vec3()> cam_data = [&gui]() { return gui.getCamera(); };

	// return value says where location of each texture is
	std::function<int()> texture_data0 = []() {return 0; };
	std::function<int()> texture_data1 = []() {return 1; };
	std::function<int()> texture_data2 = []() {return 2; };
	std::function<int()> texture_data3 = []() {return 3; };
	std::function<int()> texture_data4 = []() {return 4; };
	std::function<int()> texture_data5 = []() {return 5; };

	// create uniforms
	auto std_view = make_uniform("view", view_data);
	auto std_proj = make_uniform("projection", proj_data);
	auto std_camera = make_uniform("camera_position", cam_data);
	
	auto texture0 = make_uniform("texture0", texture_data0);
	auto texture1 = make_uniform("texture1", texture_data1);
	auto texture2 = make_uniform("texture2", texture_data2);
	auto texture3 = make_uniform("texture3", texture_data3);
	auto texture4 = make_uniform("texture4", texture_data4);
	auto texture5 = make_uniform("texture5", texture_data5);

	std::cout << "Finished!" << std::endl;

	return 0;
}