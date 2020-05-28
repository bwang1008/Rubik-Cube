#include "solver.h"
#include <map>

/*
Solver::Solver() {

}
*/

/* 
* Face is an int in [0, 6]
* Returns what the color of this face should be
*/
int Solver::getFaceColor(int face) {
	if (N % 2 == 1) {
		int index = N / 2;
		return faces[face][index][index];
	}
	return face;
}

// Gets the color of the sticker, given face, row, and column
// 0 - 6 means green, red, white, yellow, orange, blue
int Solver::getSticker(int face, int row, int col) {
	// row, col 0-indexed from top right
	return faces[face][row][col];
}

/* After Rubik's Cube is scrambled, copy sticker colors to inner representation of cube
*/
void Solver::copyConfiguration(std::vector<glm::vec3>& centers, std::vector<int>& types) {
	float half = N / 2.0f;
	for (size_t i = 0; i < types.size(); i += 8) {
		int type = types[i];
		glm::vec3 myCenter = centers[i];

		// type made of 12 bits for axes, then 6 bit bitmask of which faces are colored
		int first12Bits = (type >> 6) & 4095;
		int faceBits = type & 63;

		int frontAxisBits = (first12Bits >> 8) & 15;
		int rightAxisBits = (first12Bits >> 4) & 15;
		int topAxisBits = (first12Bits >> 0) & 15;

		int frontSign = frontAxisBits >> 3;
		int rightSign = rightAxisBits >> 3;
		int topSign = topAxisBits >> 3;

		// generate actual axes
		glm::ivec3 frontAxis = glm::ivec3((frontAxisBits >> 2) & 1, (frontAxisBits >> 1) & 1, frontAxisBits & 1);
		glm::ivec3 rightAxis = glm::ivec3((rightAxisBits >> 2) & 1, (rightAxisBits >> 1) & 1, rightAxisBits & 1);
		glm::ivec3 topAxis = glm::ivec3((topAxisBits >> 2) & 1, (topAxisBits >> 1) & 1, topAxisBits & 1);

		if (frontSign == 1)
			frontAxis = -frontAxis;
		if (rightSign == 1)
			rightAxis = -rightAxis;
		if (topSign == 1)
			topAxis = -topAxis;

		// generate a unique number for each axis
		int frontAxisNum = 9 * frontAxis[0] + 3 * frontAxis[1] + frontAxis[2];
		int rightAxisNum = 9 * rightAxis[0] + 3 * rightAxis[1] + rightAxis[2];
		int topAxisNum = 9 * topAxis[0] + 3 * topAxis[1] + topAxis[2];

		// std::vector<int> axisNums = { 1, 3, 9, -9, -3, -1 };
		std::vector<int> axisNums = { frontAxisNum, rightAxisNum, topAxisNum, -topAxisNum, -rightAxisNum, -frontAxisNum };

		// go in order of cube's front, right, up, down, left, back
		for (int j = 5; j >= 0; j--) {
			int bit = (faceBits >> j) & 1;
			if (bit == 0) // no sticker to color if face is black
				continue;
			int myColor = 5 - j; // 0 - 6 color means green, red, white, yellow, orange, blue
			int myAxisNum = axisNums[5 - j];
			int index = -1; // which face
			float x = 0.0f; // row 
			float y = 0.0f; // col

			if (myAxisNum > 0) {
				if (myAxisNum == 1) { // <0,0,1> - points in positive Z
					index = 0; // corresponds to big cube's front - faces[0]
					x = -myCenter[1];
					y = myCenter[0];
				}
				else if (myAxisNum == 3) { // <0, 1, 0> - points in positive Y
					index = 2; // corresponds to big cube's top - faces[2];
					x = myCenter[2];
					y = myCenter[0];
				}
				else { // myAxisNum == 9 - <1, 0, 0> - points in positive X
					index = 1; // corresponds to big cube's right - faces[1];
					x = -myCenter[1];
					y = -myCenter[2];
				}
			}
			else { // myAxisNum < 0: -1, -3, -9
				if (myAxisNum == -1) { // points in negative Z
					index = 5; // corresponds to big cube's back - faces[5];
					x = -myCenter[1];
					y = -myCenter[0];

				}
				else if (myAxisNum == -3) { // points in negative Y
					index = 3; // corresponds to big cube's bottom - faces[3];
					x = -myCenter[2];
					y = myCenter[0];
				}
				else { // myAxisNum == -9 - points in negative X
					index = 4; // corresponds to big cube's left - faces[4];
					x = -myCenter[1];
					y = myCenter[2];
				}
			}

			// translate x,y coordinates by half
			x += half;
			y += half;

			// fill in color
			faces[index][int(x)][int(y)] = myColor;
			
		}

	}
}

void Solver::turnFront(int layer, int qts) {
	
}

void Solver::turnRight() {

}

void Solver::turnTop() {

}

void Solver::turnBottom() {

}

void Solver::turnLeft() {

}

void Solver::turnBack() {

}

void Solver::solve() {

}

void Solver::print() {
	std::cout << "Front\n";
	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = faces[0][i][j];
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	
	std::cout << "Right\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = faces[1][i][j];
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Up\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = faces[2][i][j];
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Down\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = faces[3][i][j];
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Left\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = faces[4][i][j];
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Back\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = faces[5][i][j];
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}