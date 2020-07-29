#ifndef SKINNING_GUI_H
#define SKINNING_GUI_H

#include <glm/glm.hpp>						// GLM for vec, mat
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <chrono>							// timing
#include <deque>							// std::deque
#include <stdlib.h>

#include "config.h"							// constants
#include "solver.h"							// access to Solver

/*
 * Hint: call glUniformMatrix4fv on thest pointers
 */
struct MatrixPointers {
	const glm::mat4 *projection, *model, *view;
};

class GUI {
public:
	GUI(GLFWwindow*, int view_width = -1, int view_height = -1, int preview_height = -1);
	~GUI();

	Solver* solver;
	void loadSolver(Solver* s) { solver = s; }
	void setDeque() { solver->setDequePtr(&allMoves);  }

	void keyCallback(int key, int scancode, int action, int mods);
	void mousePosCallback(double mouse_x, double mouse_y);
	void mouseButtonCallback(int button, int action, int mods);
	void mouseScrollCallback(double dx, double dy);
	void updateMatrices();
	MatrixPointers getMatrixPointers() const;

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallback(GLFWwindow* window, double dx, double dy);

	glm::vec3 getCenter() const { return center_; }
	const glm::vec3& getCamera() const { return eye_; }

	float getEyeX() { return eye_[0]; }
	float getEyeY() { return eye_[1]; }
	float getEyeZ() { return eye_[2]; }

	glm::ivec3 getCurrentMove() { return currentMove; }
	int peekMove() {
		if (solver->currentState() % 2 == 1 || allMoves.size() == 0) {
			return -1;
		}
		else {
			return allMoves.front()[0];
		}
	}
	void setCurrentMove() {
		if(solver->currentState() % 2 == 1 || allMoves.size() == 0) {
			currentMove = glm::ivec3(-1, 0, 0);
		}
		else {
			currentMove = allMoves.front();
			allMoves.pop_front();

			countMoves++;
			countQT += std::abs(currentMove[2]);
		}
	}
	void setCurrentMove(glm::ivec3 v) { currentMove[0] = v[0]; currentMove[1] = v[1]; currentMove[2] = v[2]; }
	void addMove(glm::vec3 v) { allMoves.push_back(v); }

	const float* getLightPositionPtr() const { return &light_position_[0]; }

	bool isPlaying() const { return play_; }
	float getCurrentPlayTime();
	void setStartTime() { start_time = std::chrono::system_clock::now(); }

	bool isQuarterTurning() { return quarter_turning; }
	void setQuarterTurning(bool b) { quarter_turning = b; }
	float getRotatingSpeed() { return rotation_speed_; }
	void setRotatingSpeed(float f) { rotation_speed_ = f; }

	size_t getSize() { return allMoves.size(); }

	long long getCountMoves() { return countMoves; }
	long long getCountQT() { return countQT; }
	void resetCount() { countMoves = 0; countQT = 0; }
	
private:
	GLFWwindow* window_;

	int window_width_, window_height_;
	int view_width_, view_height_;
	int preview_height_;

	bool drag_state_ = false;
	bool fps_mode_ = false;
	int current_button_ = -1;
	float roll_speed_ = float(M_PI / 64.0f);
	float last_x_ = 0.0f, last_y_ = 0.0f, current_x_ = 0.0f, current_y_ = 0.0f;
	float camera_distance_ = cubeWidth * 3;
	//float pan_speed_ = 0.1f;
	float pan_speed_ = 0.5f;
	float rotation_speed_ = 10.0f; // how fast quarter turn rotations graphically are
	float drag_speed_ = 0.02f;
	float zoom_speed_ = 0.5f + 0.05 * cubeWidth;
	float aspect_;

	glm::vec3 eye_ = glm::vec3(0.0f, 0.1f, camera_distance_);
	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 look_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 tangent_ = glm::cross(look_, up_);
	//glm::vec3 center_ = eye_ - camera_distance_ * look_;
	glm::vec3 center_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat3 orientation_ = glm::mat3(tangent_, up_, look_);
	glm::vec4 light_position_;

	glm::mat4 view_matrix_ = glm::lookAt(eye_, center_, up_);
	glm::mat4 projection_matrix_;
	glm::mat4 model_matrix_ = glm::mat4(1.0f);

	// Face 0 = Front, 1 = Right, 2 = Top, 3 = Bottom, 4 = Left, 5 = Back
	// Which layer behind Face
	// How many quarter turns
	std::deque<glm::ivec3> allMoves; // list of all moves to perform
	glm::ivec3 currentMove = glm::ivec3(-1, 0, 0); // current move

	bool quarter_turning = false;

	bool captureWASDUPDOWN(int key, int action);

	bool play_ = true;
	double offset = 0;
	std::chrono::time_point<std::chrono::system_clock> start_time;
	float lastTime;

	long long countMoves = 0;
	long long countQT = 0;
};

#endif
