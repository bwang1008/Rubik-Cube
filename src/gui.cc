#include "gui.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>			// glm::perspective
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>							// sqrt

GUI::GUI(GLFWwindow* window, int view_width, int view_height, int preview_height) : window_(window) {
	glfwSetWindowUserPointer(window_, this);
	glfwSetKeyCallback(window_, KeyCallback);
	glfwSetCursorPosCallback(window_, MousePosCallback);
	glfwSetMouseButtonCallback(window_, MouseButtonCallback);
	glfwSetScrollCallback(window_, MouseScrollCallback);
	glfwGetWindowSize(window_, &window_width_, &window_height_);

	float aspect_ = static_cast<float>(view_width) / view_height;
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
}

GUI::~GUI() {

}

void GUI::keyCallback(int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window_, GL_TRUE);
		return;
	}
	else if (mods == 0 && captureWASDUPDOWN(key, action)) {
		return;
	}
	else if (key == GLFW_KEY_V && action != GLFW_RELEASE){
		center_ = glm::vec3(0.0f, 0.0f, 0.0f);
		up_ = glm::vec3(0.0f, 1.0f, 0.0f);
		look_ = glm::vec3(0.0f, 0.0f, -1.0f);
		tangent_ = glm::cross(look_, up_);
		orientation_ = glm::mat3(tangent_, up_, look_);
		camera_distance_ = kCubeWidth * 3;
	}
}

void GUI::mousePosCallback(double mouse_x, double mouse_y) {
	last_x_ = current_x_;
	last_y_ = current_y_;
	current_x_ = float(mouse_x);
	current_y_ = float(window_height_ - mouse_y);
	float delta_x = current_x_ - last_x_;
	float delta_y = current_y_ - last_y_;
	if (sqrt(delta_x * delta_x + delta_y * delta_y) < 1e-15) {
		return;
	}

	glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));
	glm::vec2 mouse_start = glm::vec2(last_x_, last_y_);
	glm::vec2 mouse_end = glm::vec2(current_x_, current_y_);

	bool drag_camera = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_LEFT;

	// glm::vec3 posVec = glm::unProject(glm::vec3(current_x_, current_y_, 0.0), view_matrix_, projection_matrix_, glm::vec4(0,0,window_width_,window_height_));

	if (drag_camera) {
		glm::vec3 axis = glm::normalize(orientation_ * glm::vec3(mouse_direction.y, -mouse_direction.x, 0.0f));
		
		orientation_ = glm::mat3(glm::rotate(drag_speed_, axis) * glm::mat4(orientation_));
		tangent_ = glm::column(orientation_, 0);
		up_ = glm::column(orientation_, 1);
		look_ = glm::column(orientation_, 2);
	}
}

void GUI::mouseButtonCallback(int button, int action, int mods) {
	drag_state_ = (action == GLFW_PRESS);
	current_button_ = button;
}

void GUI::mouseScrollCallback(double dx, double dy) {
	// FIXME: Mouse Scrolling
}

void GUI::updateMatrices()
{
	// Compute our view, and projection matrices.
	eye_ = center_ - camera_distance_ * look_;

	view_matrix_ = glm::lookAt(eye_, center_, up_);

	aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
	model_matrix_ = glm::mat4(1.0f);
}

MatrixPointers GUI::getMatrixPointers() const {
	MatrixPointers ret;
	ret.projection = &projection_matrix_;
	ret.model= &model_matrix_;
	ret.view = &view_matrix_;
	return ret;
}

bool GUI::captureWASDUPDOWN(int key, int action) {
	if (key == GLFW_KEY_W) {
		camera_distance_ -= zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_S) {
		camera_distance_ += zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_A) {
		center_ -= pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_D) {
		center_ += pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_DOWN) {
		center_ -= pan_speed_ * up_;
		return true;
	} else if (key == GLFW_KEY_UP) {
		center_ += pan_speed_ * up_;
		return true;
	}
	return false;
}

void GUI::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->keyCallback(key, scancode, action, mods);
}

void GUI::MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y) {
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mousePosCallback(mouse_x, mouse_y);
}

void GUI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseButtonCallback(button, action, mods);
}

void GUI::MouseScrollCallback(GLFWwindow* window, double dx, double dy) {
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseScrollCallback(dx, dy);
}