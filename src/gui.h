#ifndef GUI_H
#define GUI_H

#include <glm/glm.hpp>					// OpenGL mathematics header, for vec,mat
#include <GLFW/glfw3.h>					// GLFWwindow
#include <glm/gtc/matrix_transform.hpp> // glm::lookAt

#include "config.h"						// global constants
// #include "solver.h"						// access to Solver

struct MatrixPointers {
	const glm::mat4 *projection, *model, *view;
};

class GUI {
public:
	GUI(GLFWwindow*, int view_width = -1, int view_height = -1, int preview_height = -1);
	~GUI();

	void keyCallback(int key, int scancode, int action, int mods);
	void mousePosCallback(double mouse_x, double mouse_y);
	void mouseButtonCallback(int button, int action, int mods);
	void mouseScrollCallback(double dx, double dy);
	void updateMatrices();
	MatrixPointers getMatrixPointers() const;
	bool captureWASDUPDOWN(int key, int action);

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallback(GLFWwindow* window, double dx, double dy);

	const glm::vec3& getCamera() const;
	int getState();
	int changeState(int x = -1);

private:
	// window
	GLFWwindow* window_;
	int window_width_, window_height_;

	// mouse
	bool drag_state_ = false;
	int current_button_ = -1;
	float last_x_ = 0.0f, last_y_ = 0.0f, current_x_ = 0.0f, current_y_ = 0.0f;

	// float roll_speed_ = float(M_PI / 64.0f);
	float rotation_speed_ = 10.0f; // how fast quarter turn rotations graphically are
	
	float aspect_;

	// user speed
	float pan_speed_ = 0.1f + 0.01 * kCubeWidth;
	float zoom_speed_ = 0.1f + 0.01 * kCubeWidth;
	float drag_speed_ = 0.02f;						// mouse click drag

	// camera
	float camera_distance_ = kCubeWidth * 3;
	glm::vec3 eye_ = glm::vec3(0.0f, 0.1f, camera_distance_);
	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 look_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 tangent_ = glm::cross(look_, up_);
	glm::vec3 center_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat3 orientation_ = glm::mat3(tangent_, up_, look_);

	// matrices for openGL pipeline
	glm::mat4 view_matrix_ = glm::lookAt(eye_, center_, up_);
	glm::mat4 projection_matrix_;
	glm::mat4 model_matrix_ = glm::mat4(1.0f);
};

#endif