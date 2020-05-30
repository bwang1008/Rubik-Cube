#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <deque>
#include <glm/glm.hpp>
#include "config.h"

class Solver {
public:
	Solver();
	void setDequePtr(std::deque<glm::ivec3>* deque);
	int getFaceColor(int face);
	int getSticker(int face, int row, int col);
	void copyConfiguration(std::vector<glm::vec3>& centers, std::vector<int>& types);

	void turnFront(int layer, int qt);
	void turnRight(int layer, int qt);
	void turnUp(int layer, int qt);
	void turnDown(int layer, int qt);
	void turnLeft(int layer, int qt);
	void turnBack(int layer, int qt);

	void exec(int face, int layer, int qt);
	void exec(glm::vec3 move);

	void getPossiblePositions(int row, int col, std::vector<glm::ivec2>& container);

	void print();
	void incr();
	int currentState();
	
	void solveCenter0();

private:
	int N;
	int faces[6][cubeWidth][cubeWidth];
	// front, right, top, bottom, left, back
	// green, red, white, yellow, orange, blue

	std::deque<glm::ivec3>* dequePtr;

	int state; // current state that cube is in
	// 0 = scrambling
	// copy stickers
	// 1 = wait for ENTER key to proceed
	// 2 = solve bottom center
};

#endif