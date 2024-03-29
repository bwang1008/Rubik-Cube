#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>		// std::cout, std::endl
#include <vector>		// std::vector
#include <string>		// std::string
#include <algorithm>	// std::sort
#include <deque>		// std::deque
#include <utility>		// std::pair, std::make_pair
#include <glm/glm.hpp>	// GLM for vec, mat
#include "config.h"		// constants

class Solver {
public:
	Solver();
	void setDequePtr(std::deque<glm::ivec3>* deque);
	int getFaceColor(int face);
	int get(int face, int row, int col);
	void set(int face, int row, int col, int color);

	void turnFront(int layer, int qt);
	void turnRight(int layer, int qt);
	void turnUp(int layer, int qt);
	void turnDown(int layer, int qt);
	void turnLeft(int layer, int qt);
	void turnBack(int layer, int qt);

	void exec(int face, int layer, int qt);

	void print();
	void incr();
	int currentState();
	void scrambleCube();
	
	void preliminary0();
	void solveCenter0();
	void preliminary1();
	void solveCenter1();
	void solveCenter2();
	void solveCenter3();
	void solveLastCenters();
	
	void flipEdge(int edge);
	void solveEdges0(std::vector<std::pair<int, int>>& colorPairs);
	void solveEdges1(std::vector<std::pair<int, int>>& colorPairs);
	void solveEdges2();
	void solveEdges();

	// fix parity of even size cubes
	void fixParityEdge();
	void fixParityOppositeCorners();
	void fixParityAdjacentCorners();

	void solveCross();
	void solveCorners4();
	void solveSecondLayer();
	void solveLastCross();
	void solveLastEdges();
	void solveLastCornerPosition();
	void solveLastCornerOrientation();
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
	// 13 = wait for ENTER key to proceed
	// 14 = solve as 3x3x3

	int storeTopColor; // used for preserving color in between preliminary0 and preliminary1
	int facePlace[6]; // each value is 0,1,2,3, indicating if face is normal (0), turned cw (1), upside down (2), or ccw (3)
};

#endif
