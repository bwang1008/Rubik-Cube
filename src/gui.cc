#include "gui.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>			// glm::perspective
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

