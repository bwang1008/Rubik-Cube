#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include "config.h"

class Solver {
public:
	Solver() { N = cubeWidth;  state = 0; }
	int getFaceColor(int face);
	int getSticker(int face, int row, int col);
	void copyConfiguration(std::vector<glm::vec3>& centers, std::vector<int>& types);

	void turnFront(int layer, int qt);
	void turnRight();
	void turnTop();
	void turnBottom();
	void turnLeft();
	void turnBack();

	void solve();
	void print();
	void incr() { state++; }
	int currentState() { return state; }
	

private:
	int N;
	int faces[6][cubeWidth][cubeWidth];
	// front, right, top, bottom, left, back
	// green, red, white, yellow, orange, blue

	int state;
};

#endif