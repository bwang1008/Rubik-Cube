#include <GL/glew.h>

#include "procedure_geometry.h"
#include "render_pass.h"
#include "config.h"
#include "gui.h"
#include "texture_to_render.h"
#include "cube.h"

#include <memory>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <debuggl.h>

#include <jpegio.h>
//#include <math.h>
#include <cmath>


int window_width = 720; //1280;
int window_height = 720;

const std::string window_title = "Rubik's Cube";

const char* vertex_shader =
#include "shaders/default.vert"
;

const char* blending_shader =
#include "shaders/blending.vert"
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

const char* sky_vertex_shader = 
#include "shaders/sky.vert"
;

const char* sky_fragment_shader = 
#include "shaders/sky.frag"
;


void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw()
{
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


int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Input model file is missing" << std::endl;
		std::cerr << "Usage: " << argv[0] << " <PMD file>" << std::endl;
		return -1;
	}
	if (cubeWidth <= 0 || cubeWidth > 200) {
		std::cerr << "Cube width defined in config.h is invalid" << std::endl;
		std::cerr << "Width cannot be negative, and large values consume too much resources" << std::endl;
		return -1;
	}

	std::cout << "Cube Width = " << cubeWidth << std::endl;

	GLFWwindow *window = init_glefw();
	GUI gui(window, window_width, window_height, window_height);

	// tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	matX2[2][2] = 1.0f;

	matY2[0][0] = -1.0f;
	matY2[2][0] = 0.0f;
	matY2[0][2] = 0.0f;
	matY2[2][2] = 1.0f;

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

	// Information for cubes
	std::vector<Cube*> cubes; // actual Cube objects
	std::vector<glm::vec4> cube_vertices; // list of all vertices of cubes in world coord
	std::vector<glm::uvec3> cube_faces; // all triangular faces making up all cubes
	std::vector<int> cube_types; // specify which faces of cube to actually color
	std::vector<glm::vec3> cube_centers; // centers of each cube

	// Pass containers into method to populate the vectors appropriately 
	create_rubik(cubes, cube_vertices, cube_faces, cube_types, cube_centers);

	std::vector<int> cube_rotating; // which cubes should be rotating with gui.getCurrentMove()
	for(size_t i = 0; i < 8 * cubes.size(); ++i) {
		cube_rotating.push_back(0);
	}

	std::cout << "Num cubes = " << cubes.size() << std::endl;
	std::cout << "Num vertices = " << cube_vertices.size() << std::endl;
	std::cout << "Num faces  = " << cube_faces.size() << std::endl;

	// SKY BOX
	std::vector<glm::vec4> sky_vertices;
	std::vector<glm::uvec3> sky_faces;
	create_skybox(sky_vertices, sky_faces, gui.getCamera());

	glm::vec4 light_position = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);

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

	// FIXME: add more lambdas for data_source if you want to use RenderPass.
	//        Otherwise, do whatever you like here

	std::function<glm::mat4()> view_data = [&mats]() { return *mats.view; };
	std::function<glm::mat4()> proj_data = [&mats]() { return *mats.projection; };
	std::function<glm::mat4()> identity_mat = [](){ return glm::mat4(1.0f); };
	std::function<glm::vec3()> cam_data = [&gui](){ return gui.getCamera(); };
	std::function<glm::vec4()> lp_data = [&light_position]() { return light_position; };
	std::function<int()> face_data = [&gui]() { return gui.getCurrentMove()[0]; };
	std::function<float()> theta_data = [&gui]() { return -gui.getCurrentPlayTime() * gui.getRotatingSpeed(); };

	auto std_view = make_uniform("view", view_data);
	auto std_camera = make_uniform("camera_position", cam_data);
	auto std_proj = make_uniform("projection", proj_data);
	auto std_light = make_uniform("light_position", lp_data);
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

	RenderDataInput sky_pass_input;
	sky_pass_input.assign(0, "vertex_position", sky_vertices.data(), sky_faces.size(), 4, GL_FLOAT);
	sky_pass_input.assignIndex(sky_faces.data(), sky_faces.size(), 3);
	RenderPass sky_pass(-1, sky_pass_input,
			{ sky_vertex_shader, nullptr, sky_fragment_shader},
			{ std_view, std_proj},
			{ "fragment_color"}
			);


	bool draw_cube = true;
	bool draw_sky = true;

	if(argc == 3){
		std::string s(argv[2]);
		std::cout << "loading file: " << s << std::endl;
		//mesh.loadAnimationFrom(s);
	}
	/*
	if (argc >= 3) {
		mesh.loadAnimationFrom(argv[2]);
	}
	*/

	gui.addMove(glm::vec3(0, 0, 2));
	gui.addMove(glm::vec3(2, 0, 1));
	gui.addMove(glm::vec3(1, 1, 1));

	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
		else
		{
			title << " Paused: "
				<< std::setprecision(2)
				<< std::setfill('0') << std::setw(6)
				<< cur_time << " sec";
		}

		glfwSetWindowTitle(window, title.str().data());
		glViewport(0, 0, window_width, window_height);

		// update rotation, depending if rotating already or not
		if(gui.isQuarterTurning()) {

			float currentTheta = std::abs(gui.getCurrentPlayTime() * gui.getRotatingSpeed());
			glm::ivec3 myMove = gui.getCurrentMove();
			int face = myMove[0];
			int turns = ((myMove[2] % 4) + 4) % 4;
			float supposedRadians = (std::abs(myMove[2])) * 3.1415926/2.0;

			if(currentTheta > supposedRadians && face >= 0) { // if it turned enough
				std::cout << "currentTheta = " << currentTheta << std::endl;
				std::cout << "currentPlayTime * rotateSpeed = " << gui.getCurrentPlayTime() << " * " << gui.getRotatingSpeed() << std::endl;
				std::cout << "supposedRadians = " << supposedRadians << std::endl;
				// update cube vertices (VBO) and centers after making quarter turns
				// to make transformations permanent
				glm::mat4 mat = glm::mat4(1.0f);

				if(face == 0) { // front
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
				if(face == 5) { // back
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

				std::cout << "mat = " << std::endl;
				for(int i = 0; i < 4; ++i) {
					for(int j = 0; j < 4; ++j) {
						std::cout << mat[j][i] << " ";
					}
					std::cout << std::endl;
				}
				

				for(size_t i = 0; i < cube_centers.size(); i += 8) {
					if(cube_rotating[i] == 1) {
						glm::vec4 origPos = glm::vec4(cube_centers[i], 1.0f);
						glm::vec4 newPos = mat * origPos;

						for(int j = 0; j < 8; ++j) {
							cube_centers[i + j] = newPos;
						}

						cube_vertices[i + 0] = glm::vec4(newPos[0] - 0.5, newPos[0] - 0.5, newPos[0] - 0.5, 1);
						cube_vertices[i + 1] = glm::vec4(newPos[0] + 0.5, newPos[0] - 0.5, newPos[0] - 0.5, 1);
						cube_vertices[i + 2] = glm::vec4(newPos[0] - 0.5, newPos[0] + 0.5, newPos[0] - 0.5, 1);
						cube_vertices[i + 3] = glm::vec4(newPos[0] + 0.5, newPos[0] + 0.5, newPos[0] - 0.5, 1);
						cube_vertices[i + 4] = glm::vec4(newPos[0] - 0.5, newPos[0] - 0.5, newPos[0] + 0.5, 1);
						cube_vertices[i + 5] = glm::vec4(newPos[0] + 0.5, newPos[0] - 0.5, newPos[0] + 0.5, 1);
						cube_vertices[i + 6] = glm::vec4(newPos[0] - 0.5, newPos[0] + 0.5, newPos[0] + 0.5, 1);
						cube_vertices[i + 7] = glm::vec4(newPos[0] + 0.5, newPos[0] + 0.5, newPos[0] + 0.5, 1);
					}
				}

				// updateVBO of vertices
				cube_pass.updateVBO(0, cube_vertices.data(), cube_vertices.size());



				gui.setQuarterTurning(false);
			}
		}
		if(!gui.isQuarterTurning()) {

			gui.setQuarterTurning(true); // Applying next move

			//gui.addMove(glm::vec3(2, 0, 1));

			gui.setCurrentMove(); // Get the next move

			glm::ivec3 myMove = gui.getCurrentMove();
			std::cout << "myMove = " << myMove[0] << " " << myMove[1] << " " << myMove[2] << std::endl;

			gui.setStartTime(); // set time at 0
			update_rubik2(cube_centers, gui.getCurrentMove(), cube_rotating); // find which cubes should be rotating
			std::cout << "time = " << gui.getCurrentPlayTime() << std::endl;
			
			cube_pass.updateVBO(2, cube_rotating.data(), cube_rotating.size());
		}

		// Render cubes
		if (draw_cube) {
			cube_pass.setup();
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, cube_faces.size() * 3, GL_UNSIGNED_INT, 0));
		}

		// Render skybox
		if (draw_sky) {
			sky_pass.setup();
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, sky_faces.size() * 3, GL_UNSIGNED_INT, 0));
		}

		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// free memory
	for(auto ptr : cubes)
		delete ptr;
	cubes.clear();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);


}
