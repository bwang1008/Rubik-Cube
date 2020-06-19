#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>		// std::cout, std::endl
#include <vector>		// std::vector
#include <string>		// std::string
#include <algorithm>	// std::sort
#include <deque>		// std::deque
#include <utility>		// std::pair, std::make_pair
#include <glm/glm.hpp>
#include "config.h"		// constants

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
	void solveCenter1();
	void solveCenter2();
	void solveCenter3();
	void solveLastCenters();
	
	void flipEdge(int edge);
	void solveEdges0(std::vector<std::pair<int, int>>& colorPairs);
	void solveEdges1(std::vector<std::pair<int, int>>& colorPairs);
	void solveEdges2();
	void solveEdges();

	void solveCross();
	void solveCorners4();
	void solve3x3x3();


private:
	int N;
	int faces[6][cubeWidth][cubeWidth]; // internal representation
	// 0 1 2 3 4 5
	// front, right, top, bottom, left, back
	// green, red, white, yellow, orange, blue

	std::deque<glm::ivec3>* dequePtr; // pointer to the deque of moves to fill

	int state; // current state that cube is in
	// 0 = scrambling
	// copy stickers
	// 1 = wait for ENTER key to proceed
	// 2 = solve bottom center
	// 3 = wait for ENTER key to proceed
	// 4 = solve top center
	// 5 = wait for ENTER key to proceed
	// 6 = solve back center
	// 7 = wait for ENTER key to proceed
	// 8 = solve left center
	// 9 = wait for ENTER key to proceed
	// 10 = solve front and right face simultaneously
	// 11 = wait for ENTER key to proceed
	// 12 = solve edges

};

#endif