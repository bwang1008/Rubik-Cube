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
	
	int getFaceColor(int face);
	int get(int face, int row, int col);
	void set(int face, int row, int col, int color);
	void dequeAdd(int face, int layer, int qt);
	int dequeSize();

	void print();
	void scrambleCube();

	void solve();

private:
	int N;
	int faces[6][kCubeWidth][kCubeWidth]; // internal representation
	// 0 1 2 3 4 5
	// front, right, top, bottom, left, back
	// green, red, white, yellow, orange, blue

	std::deque<glm::ivec3> deq; // deque of moves to fill
	int storeTopColor;		// temporary storage variable in between methods when solving back face on top face
	int facePos[6];		// 0 means unrotated, 1 means rotated clockwise from original, 2 means rotated twice, 3 means rotated once counterclockwise

	void turnFront(int layer, int qt);
	void turnRight(int layer, int qt);
	void turnUp(int layer, int qt);

	void exec(int face, int layer, int qt);

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
};

#endif
