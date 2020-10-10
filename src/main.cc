#include <GL/glew.h> 				// libglew-dev
					
#include "config.h"					// global constants					
#include "gui.h"					// gui handles keyboard/mouse
#include "handle_json.h"			// read & write json files
#include "procedure_geometry.h"		// generate all 3D objects
#include "render_pass.h"			// helper for rendering in OpenGL
#include "solver.h"					// Solver object -> algorithms to solve scrambled Rubik's Cube

#include <iostream>					// std::cout, std::endl, std::cerr
#include <sstream> 					// std::stringstream
#include <vector>					// std::vector

#define N kCubeWidth				// just because N is shorter to type

int window_width =  600;
int window_height = 600;

const std::string window_title = "Rubik's Cube";

// Load shaders
const char* cube_vertex_shader = 
#include "shaders/preview.vert"
;

const char* cube_fragment_shader = 
#include "shaders/preview.frag"
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
	std::vector<int> cube_id;

	create_large_cube(cube_vertices, cube_faces);					// procedure_geometry::create_large_cube to fill in vertices and faces of overall rubik's cube

	for(int i = 0; i < 6; ++i)										// have 0,1,2,3,4,5, to represent the 6 faces
		for(int j = 0; j < 4; ++j)									// repeat four times...? (trial and error guessed) I think because we don't have geometry shader 
			cube_id.push_back(i);

	Image* images[6];												// 6 different textures for each face of cube
	Solver* solver = new Solver();									// Solver container holds colors of all 6 faces
	std::vector<glm::uvec3> rgbs;									// map from int (0-5) to rgb

	// user arguments
	if(argc >= 2) {
		std::string path(argv[1]);										
		int res = read_json(path, solver, rgbs);					// handle_json::read_json
		if(res < 0) {
			return -1;
		}

		/*
		for(int i = 0; i < 6; ++i) {
			std::cout << "well = " << rgbs[i][0] << " " << rgbs[i][1] << " " << rgbs[i][2] << std::endl;
		}
		*/
	}
	else {
		// no user arguments; prepopulate rgbs with the default 6 colors of stickers: green, red, white, yellow, orange, blue 
		// solver is already prepopulated with default in Solver constructor
		rgbs.push_back(glm::uvec3(46, 139, 87));
		rgbs.push_back(glm::uvec3(255, 0, 0));
		rgbs.push_back(glm::uvec3(255, 255, 255));
		rgbs.push_back(glm::uvec3(255, 255, 0));
		rgbs.push_back(glm::uvec3(255, 140, 0));
		rgbs.push_back(glm::uvec3(0, 0, 255));
	}
	// end user arguments

	create_textures(images, solver, rgbs);							// procedure_geoemetry::create_textures to fill in images with correct bytes
	
	unsigned int textureNum[6];										// generated number for each texture (?)
	for (int i = 0; i < 6; ++i) {									
		glGenTextures(1, &(textureNum[i]));							// create actual number
		glBindTexture(GL_TEXTURE_2D, textureNum[i]);						
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Snap to nearest pixel
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, images[i]->width, images[i]->height, 0, GL_RGB, GL_UNSIGNED_BYTE, images[i]->bytes.data());	// generate actual texture
		glGenerateMipmap(GL_TEXTURE_2D);							// mipmaps for when zooming in or out

		std::cout << "Generated image " << (i+1) << std::endl;
	}
	
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

	
	RenderDataInput cube_pass_input;
	cube_pass_input.assign(0, "vertex_position", cube_vertices.data(), cube_vertices.size(), 4, GL_FLOAT);
	cube_pass_input.assign(1, "my_face", cube_id.data(), cube_id.size(), 1, GL_INT);
	cube_pass_input.assignIndex(cube_faces.data(), cube_faces.size(), 3);

	RenderPass cube_pass(-1, cube_pass_input, 
		{cube_vertex_shader, nullptr, cube_fragment_shader},									// vertex shader, geometry shader, fragment shader
		{ std_view, std_proj, texture0, texture1, texture2, texture3, texture4, texture5}, 		// uniforms
		{ "fragment_color" }																	// name of output of shader
	);
	
	std::cout << "Rendering!" << std::endl;

	while(!glfwWindowShouldClose(window)) {
		// setup "basic" window stuff
		glfwGetFramebufferSize(window, &window_width, &window_height);	// "size, in pixels, of the framebuffer of the specified window" - stored in window's window_width/height
		glViewport(0, 0, window_width, window_height);					// "specifies the affine transformation of x and y from normalized device coordinates to window coordinates"
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f); 							// when glClear() clears buffer, set default to this color
	
		// glEnable — enable or disable server-side GL capabilities
		glEnable(GL_DEPTH_TEST);										// I think tells framebuffer to update when there is a closer depth
		glEnable(GL_MULTISAMPLE);										// get anti-aliasing by sampling around by a small offset to get smooth transitions
		glEnable(GL_BLEND);												// "blend the computed fragment color values with the values in the color buffers"
		glEnable(GL_CULL_FACE);											// "cull polygons based on their winding in window coordinates"

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// set buffer to default values
		
		glDepthFunc(GL_LESS);											// depth comparison function: GL_LESS = want objects closer to screen (needs Depth testing available: glEnable(GL_DEPTH_TEST))
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);				// needs glEnable(GL_BLEND): blends incoming fragment colors with colors already in frame buffer
		glCullFace(GL_BACK);											// back-facing polygons are culled

		// now update cube stuff
		gui.updateMatrices();						// eye, view_matrix, projection_matrix
		mats = gui.getMatrixPointers();				// copy updated values into mats

		std::stringstream title;						// title for my window
		title << window_title;
		title << N;

		glfwSetWindowTitle(window, title.str().data()); // set title of window


		// draw cubes
		cube_pass.setup();
		CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, cube_faces.size() * 3, GL_UNSIGNED_INT, 0));	// draw primitives




	}


	std::cout << "Finished!" << std::endl;

	return 0;
}