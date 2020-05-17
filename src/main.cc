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
#include <math.h>

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

	GLFWwindow *window = init_glefw();
	GUI gui(window, window_width, window_height, window_height);

	// tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Information for cubes
	std::vector<Cube*> cubes; // actual Cube objects
	std::vector<glm::vec4> cube_vertices; // list of all vertices of cubes in world coord
	std::vector<glm::uvec3> cube_faces; // all triangular faces making up all cubes
	std::vector<int> cube_types; // specify which faces of cube to actually color

	std::vector<glm::mat4> trans;

	// Pass containers into method to populate the vectors appropriately 
	create_rubik(cubes, cube_vertices, cube_faces, cube_types, trans);

	std::vector<glm::vec4> col0s;
	std::vector<glm::vec4> col1s;
	std::vector<glm::vec4> col2s;
	std::vector<glm::vec4> col3s;

	for(glm::mat4 mat : trans) {
		col0s.push_back(mat[0]);
		col1s.push_back(mat[1]);
		col2s.push_back(mat[2]);
		col3s.push_back(mat[3]);
	}
	
	std::cout << "Num cubes = " << cubes.size() << std::endl;
	std::cout << "Num vertices = " << cube_vertices.size() << std::endl;
	std::cout << "Num faces  = " << cube_faces.size() << std::endl;
	std::cout << "trans size = " << trans.size() << std::endl;

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
	
	/*
	std::function<const glm::mat4*()> model_data = [&mats]() {
		return mats.model;
	};*/

	std::function<glm::mat4()> view_data = [&mats]() { return *mats.view; };
	std::function<glm::mat4()> proj_data = [&mats]() { return *mats.projection; };
	std::function<glm::mat4()> identity_mat = [](){ return glm::mat4(1.0f); };
	std::function<glm::vec3()> cam_data = [&gui](){ return gui.getCamera(); };
	std::function<glm::vec4()> lp_data = [&light_position]() { return light_position; };

	//auto std_model = std::make_shared<ShaderUniform<const glm::mat4*>>("model", model_data);
	auto std_view = make_uniform("view", view_data);
	auto std_camera = make_uniform("camera_position", cam_data);
	auto std_proj = make_uniform("projection", proj_data);
	auto std_light = make_uniform("light_position", lp_data);

	RenderDataInput cube_pass_input;
	cube_pass_input.assign(0, "vertex_position", cube_vertices.data(), cube_vertices.size(), 4, GL_FLOAT);
	cube_pass_input.assign(1, "cube_type", cube_types.data(), cube_types.size(), 1, GL_INT);
	cube_pass_input.assign(2, "col0", col0s.data(), col0s.size(), 4, GL_FLOAT);
	cube_pass_input.assign(3, "col1", col1s.data(), col1s.size(), 4, GL_FLOAT);
	cube_pass_input.assign(4, "col2", col2s.data(), col2s.size(), 4, GL_FLOAT);
	cube_pass_input.assign(5, "col3", col3s.data(), col3s.size(), 4, GL_FLOAT);
	cube_pass_input.assignIndex(cube_faces.data(), cube_faces.size(), 3);
	RenderPass cube_pass(-1, cube_pass_input,
			{ cube_vertex_shader, nullptr, cube_fragment_shader},
			{ std_view, std_proj},
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

		// Update transformation of vertices
		update_rubik(cubes, trans, gui.getCurrentMove());
		// update cols

		for(size_t i = 0; i < trans.size(); ++i) {
			glm::mat4 mat = trans[i];
			col0s[i] = mat[0];
			col1s[i] = mat[1];
			col2s[i] = mat[2];
			col3s[i] = mat[3];
		}

		
		// update VBOs
		cube_pass.updateVBO(2, col0s.data(), col0s.size());
		cube_pass.updateVBO(3, col1s.data(), col1s.size());
		cube_pass.updateVBO(4, col2s.data(), col2s.size());
		cube_pass.updateVBO(5, col3s.data(), col3s.size());
		

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
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
