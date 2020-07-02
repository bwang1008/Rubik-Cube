#include "solver.h"
#include <map>		// std::map for debugging purposes

Solver::Solver() {
	N = cubeWidth;
	state = 0;
	faces[0][0][0] = 0;
	dequePtr = nullptr;
}

// gives Solver access to deque of moves
void Solver::setDequePtr(std::deque<glm::ivec3>* deque) {
	dequePtr = deque;
}

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
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[0][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[0][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[0][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[0][i][j] = temp[i][j];
			}
		}
	}
	else if (layer == N - 1) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[5][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[5][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[5][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[5][i][j] = temp[i][j];
			}
		}
	}

	// now move the bands 
	int upBand[cubeWidth];
	int rightBand[cubeWidth];
	int downBand[cubeWidth];
	int leftBand[cubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = faces[2][N - 1 - layer][i];
		rightBand[i] = faces[1][i][layer];
		downBand[i] = faces[3][layer][N - 1 - i];
		leftBand[i] = faces[4][N - 1 - i][N - 1 - layer];
	}
	
	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			faces[2][N - 1 - layer][i] = leftBand[i];
			faces[1][i][layer] = upBand[i];
			faces[3][layer][N - 1 - i] = rightBand[i];
			faces[4][N - 1 - i][N - 1 - layer] = downBand[i];
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			faces[2][N - 1 - layer][i] = downBand[i];
			faces[1][i][layer] = leftBand[i];
			faces[3][layer][N - 1 - i] = upBand[i];
			faces[4][N - 1 - i][N - 1 - layer] = rightBand[i];
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			faces[2][N - 1 - layer][i] = rightBand[i];
			faces[1][i][layer] = downBand[i];
			faces[3][layer][N - 1 - i] = leftBand[i];
			faces[4][N - 1 - i][N - 1 - layer] = upBand[i];
		}
	}
	
}

void Solver::turnRight(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[1][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[1][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[1][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[1][i][j] = temp[i][j];
			}
		}
	}
	else if (layer == N - 1) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[4][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[4][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[4][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[4][i][j] = temp[i][j];
			}
		}
	}

	// now move the bands 
	int upBand[cubeWidth];
	int backBand[cubeWidth];
	int downBand[cubeWidth];
	int frontBand[cubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = faces[2][N - 1 - i][N - 1 - layer];
		backBand[i] = faces[5][i][layer];
		downBand[i] = faces[3][N - 1 - i][N - 1 - layer];
		frontBand[i] = faces[0][N - 1 - i][N - 1 - layer];
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			faces[2][N - 1 - i][N - 1 - layer] = frontBand[i];
			faces[5][i][layer] = upBand[i];
			faces[3][N - 1 - i][N - 1 - layer] = backBand[i];
			faces[0][N - 1 - i][N - 1 - layer] = downBand[i];
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			faces[2][N - 1 - i][N - 1 - layer] = downBand[i];
			faces[5][i][layer] = frontBand[i];
			faces[3][N - 1 - i][N - 1 - layer] = upBand[i];
			faces[0][N - 1 - i][N - 1 - layer] = backBand[i];
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			faces[2][N - 1 - i][N - 1 - layer] = backBand[i];
			faces[5][i][layer] = downBand[i];
			faces[3][N - 1 - i][N - 1 - layer] = frontBand[i];
			faces[0][N - 1 - i][N - 1 - layer] = upBand[i];
		}
	}
}

void Solver::turnUp(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[2][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[2][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[2][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[2][i][j] = temp[i][j];
			}
		}
	}
	else if (layer == N - 1) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[3][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[3][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[3][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[3][i][j] = temp[i][j];
			}
		}
	}

	// now move the bands 
	int backBand[cubeWidth];
	int rightBand[cubeWidth];
	int frontBand[cubeWidth];
	int leftBand[cubeWidth];

	for (int i = 0; i < N; ++i) {
		backBand[i] = faces[5][layer][N - 1 - i];
		rightBand[i] = faces[1][layer][N - 1 - i];
		frontBand[i] = faces[0][layer][N - 1 - i];
		leftBand[i] = faces[4][layer][N - 1 - i];
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			faces[5][layer][N - 1 - i] = leftBand[i];
			faces[1][layer][N - 1 - i] = backBand[i];
			faces[0][layer][N - 1 - i] = rightBand[i];
			faces[4][layer][N - 1 - i] = frontBand[i];
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			faces[5][layer][N - 1 - i] = frontBand[i];
			faces[1][layer][N - 1 - i] = leftBand[i];
			faces[0][layer][N - 1 - i] = backBand[i];
			faces[4][layer][N - 1 - i] = rightBand[i];
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			faces[5][layer][N - 1 - i] = rightBand[i];
			faces[1][layer][N - 1 - i] = frontBand[i];
			faces[0][layer][N - 1 - i] = leftBand[i];
			faces[4][layer][N - 1 - i] = backBand[i];
		}
	}
}

void Solver::turnDown(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[3][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[3][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[3][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[3][i][j] = temp[i][j];
			}
		}
	}
	else if (layer == N - 1) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[2][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[2][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[2][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[2][i][j] = temp[i][j];
			}
		}
	}

	// now move the bands 
	int frontBand[cubeWidth];
	int rightBand[cubeWidth];
	int backBand[cubeWidth];
	int leftBand[cubeWidth];

	for (int i = 0; i < N; ++i) {
		frontBand[i] = faces[0][N - 1 - layer][i];
		rightBand[i] = faces[1][N - 1 - layer][i];
		backBand[i] = faces[5][N - 1 - layer][i];
		leftBand[i] = faces[4][N - 1 - layer][i];
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			faces[0][N - 1 - layer][i] = leftBand[i];
			faces[1][N - 1 - layer][i] = frontBand[i];
			faces[5][N - 1 - layer][i] = rightBand[i];
			faces[4][N - 1 - layer][i] = backBand[i];
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			faces[0][N - 1 - layer][i] = backBand[i];
			faces[1][N - 1 - layer][i] = leftBand[i];
			faces[5][N - 1 - layer][i] = frontBand[i];
			faces[4][N - 1 - layer][i] = rightBand[i];
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			faces[0][N - 1 - layer][i] = rightBand[i];
			faces[1][N - 1 - layer][i] = backBand[i];
			faces[5][N - 1 - layer][i] = leftBand[i];
			faces[4][N - 1 - layer][i] = frontBand[i];
		}
	}
}

void Solver::turnLeft(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[4][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[4][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[4][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[4][i][j] = temp[i][j];
			}
		}
	}
	else if (layer == N - 1) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[1][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[1][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[1][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[1][i][j] = temp[i][j];
			}
		}
	}

	// now move the bands 
	int upBand[cubeWidth];
	int frontBand[cubeWidth];
	int downBand[cubeWidth];
	int backBand[cubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = faces[2][i][layer];
		frontBand[i] = faces[0][i][layer];
		downBand[i] = faces[3][i][layer];
		backBand[i] = faces[5][N - 1 - i][N - 1 - layer];
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			faces[2][i][layer] = backBand[i];
			faces[0][i][layer] = upBand[i];
			faces[3][i][layer] = frontBand[i];
			faces[5][N - 1 - i][N - 1 - layer] = downBand[i];
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			faces[2][i][layer] = downBand[i];
			faces[0][i][layer] = backBand[i];
			faces[3][i][layer] = upBand[i];
			faces[5][N - 1 - i][N - 1 - layer] = frontBand[i];
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			faces[2][i][layer] = frontBand[i];
			faces[0][i][layer] = downBand[i];
			faces[3][i][layer] = backBand[i];
			faces[5][N - 1 - i][N - 1 - layer] = upBand[i];
		}
	}
}

void Solver::turnBack(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[5][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[5][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[5][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[5][i][j] = temp[i][j];
			}
		}
	}
	else if (layer == N - 1) {
		int temp[cubeWidth][cubeWidth];
		if (qts == 1) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - j][i] = faces[0][i][j];
				}
			}
		}
		else if (qts == 2) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[N - 1 - i][N - 1 - j] = faces[0][i][j];
				}
			}
		}
		else if (qts == 3) {
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					temp[j][N - 1 - i] = faces[0][i][j];
				}
			}
		}

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				faces[0][i][j] = temp[i][j];
			}
		}
	}

	// now move the bands 
	int upBand[cubeWidth];
	int leftBand[cubeWidth];
	int downBand[cubeWidth];
	int rightBand[cubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = faces[2][layer][N - 1 - i];
		leftBand[i] = faces[4][i][layer];
		downBand[i] = faces[3][N - 1 - layer][i];
		rightBand[i] = faces[1][N - 1 - i][N - 1 - layer];
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			faces[2][layer][N - 1 - i] = rightBand[i];
			faces[4][i][layer] = upBand[i];
			faces[3][N - 1 - layer][i] = leftBand[i];
			faces[1][N - 1 - i][N - 1 - layer] = downBand[i];
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			faces[2][layer][N - 1 - i] = downBand[i];
			faces[4][i][layer] = rightBand[i];
			faces[3][N - 1 - layer][i] = upBand[i];
			faces[1][N - 1 - i][N - 1 - layer] = leftBand[i];
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			faces[2][layer][N - 1 - i] = leftBand[i];
			faces[4][i][layer] = downBand[i];
			faces[3][N - 1 - layer][i] = rightBand[i];
			faces[1][N - 1 - i][N - 1 - layer] = upBand[i];
		}
	}
}

// wrapper function that turns internal representation, and writes to deque of moves
void Solver::exec(int face, int layer, int qt) {
	switch (face) {
	case 0: turnFront(layer, qt); break;
	case 1: turnRight(layer, qt); break;
	case 2: turnUp(layer, qt); break;
	case 3: turnDown(layer, qt); break;
	case 4: turnLeft(layer, qt); break;
	case 5: turnBack(layer, qt); break;
	}

	if (OPTIMIZE && dequePtr->size() > 0) {
		glm::ivec3 copy = dequePtr->back();

		if (copy[0] == face && copy[1] == layer) {
			dequePtr->pop_back();
			copy[2] = (((copy[2] + qt) % 4) + 4) % 4;
			if (copy[2] == 3) {
				copy[2] = -1;
			}
			if (copy[2] != 0) {
				dequePtr->push_back(copy);
			}
		}
		else {
			if (qt == 3)
				qt = -1;
			dequePtr->push_back(glm::ivec3(face, layer, qt));
		}

	}
	else {
		if (qt == 3)
			qt = -1;

		if(qt != 0)
			dequePtr->push_back(glm::ivec3(face, layer, qt));
	}
}

void Solver::exec(glm::vec3 move) {
	int face = move[0];
	int layer = move[1];
	int qt = move[2];
	switch (face) {
	case 0: turnFront(layer, qt); break;
	case 1: turnRight(layer, qt); break;
	case 2: turnUp(layer, qt); break;
	case 3: turnDown(layer, qt); break;
	case 4: turnLeft(layer, qt); break;
	case 5: turnBack(layer, qt); break;
	}

	// not optimized yet
	dequePtr->push_back(move);
}

// get the 4 possible positions that a sticker at (row,col) can take,
// if the face it is on can be rotated
// in a sense, these 4 positions are equivalent (kinda?)
void Solver::getPossiblePositions(int row, int col, std::vector<glm::ivec2>& container) {
	if (row == col && row == N / 2 && N % 2 == 1) {
		container.push_back(glm::ivec2(row, col));
		return;
	}

	container.push_back(glm::ivec2(row, col));
	container.push_back(glm::ivec2(col, N - 1 - row));
	container.push_back(glm::ivec2(N - 1 - row, N - 1 - col));
	container.push_back(glm::ivec2(N - 1 - col, row));
}

// print out internal representation
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

// increment the current state of the solver
// Odd = pause, 0 = scrambling, 2 = solving first center, 4 = solving second center, ...
void Solver::incr() {
	state++;
}

// get the current state
int Solver::currentState() {
	return state;
}

// solves the Down face of the cube (for even sized cubes, it is yellow)
void Solver::solveCenter0() {
	int color = getFaceColor(3); // let's solve the bottom/down face
	
	for (int i = 1; i < N - 1; ++i) {
		int band[cubeWidth];
		for (int j = 0; j < N; ++j) {
			band[j] = faces[3][N - 1 - j][i];
		}

		// bring this band/layer up onto Front face
		exec(4, i, -1);

		// turn band so it is parallel to down
		exec(0, 0, -1);

		bool completed = false;
		while (!completed) {

			// update stickers in band
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
			}

			// get stickers from front
			for (int j = 1; j < N - 1; ++j) {
				if (band[j] != color) {
					int row = N - 1 - i;
					int col = N - 1 - j;
					std::vector<glm::ivec2> possible;
					getPossiblePositions(row, col, possible);
					for (size_t k = 0; k < possible.size(); ++k) {
						glm::ivec2 temp = possible[k];
						int r = temp[0];
						int c = temp[1];

						// if the down face, at those possible 4 locations, has the color desired,
						// and is not in same row as the band is currently in
						if (faces[0][r][c] == color && r != row) {
							// current position of sticker on front face
							int r2 = r;
							int c2 = c;

							// now rotate the desired sticker from front to right
							exec(2, r2, -1);

							// need to undo above
							int qt = 1;
							if (N - 1 - c2 != r2) {
								qt = -1;
							}

							exec(1, 0, qt); // first rotate right face out of the way
							exec(2, r2, 1); // undo above
							exec(1, 0, -qt); // undo getting out of way


							// rotate right face until in right position
							// while the row of desired sticker != jth item of band in pos (j, N-1-i)
							while (r2 != j || c2 != N - 1 - i) {
								exec(1, 0, 1);
								int r3 = c2;
								int c3 = N - 1 - r2;
								r2 = r3;
								c2 = c3;
							}

							// put band, which is currently facing bottom, to face right
							exec(0, 0, -1);

							// move that sticker into the band
							exec(2, j, 1);

							// put band, which is parallel to right, down so it is parallel to down
							exec(0, 0, 1);

							break; // we only need one sticker
						}
					}
				}
			}

			// update stickers in band
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
			}

			// rotate band until parallel to left
			exec(0, 0, 1);

			// get appropriate stickers from Left face
			for (int j = 1; j < N - 1; ++j) { // find appropriate missing stickers
				if (band[j] != color) {
					int row = N - 1 - j;
					int col = i;
					std::vector<glm::ivec2> possible;
					getPossiblePositions(row, col, possible);
					for (size_t k = 0; k < possible.size(); ++k) {
						glm::ivec2 temp = possible[k];
						int r = temp[0];
						int c = temp[1];

						// if the left face, at those possible 4 locations, has the color desired,
						if (faces[4][r][c] == color) {
							// if k > 0, may need to rotate Left face to get the sticker in desired position before sticking in band
							exec(4, 0, -k);

							// rotate sticker into band
							exec(2, row, -1);

							int qt = 1;
							if (N - 1 - col != row) {
								qt = -1;
							}

							// first rotate front before
							exec(0, 0, qt);
							// turning back the above above rotation
							exec(2, row, 1);
							exec(0, 0, -qt);

							break; // we only need one sticker in this position
						}
					}
				}
			}

			// get stickers from Up face
			// first turn front face
			exec(0, 0, 1);
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][i][j];
			}

			for (int j = 1; j < N - 1; ++j) {
				if (band[j] != color) {
					int row = i;
					int col = j;
					std::vector<glm::ivec2> possible;
					getPossiblePositions(row, col, possible);
					for (size_t k = 0; k < possible.size(); ++k) {
						glm::ivec2 temp = possible[k];
						int r = temp[0];
						int c = temp[1];

						// if we have desired sticker on Up face,
						if (faces[2][r][c] == color) {
							// rotate Up face to get the sticker in desired position
							exec(2, 0, -k);

							// rotate sticker into band
							exec(4, j, 1);

							// since rotating band down might mess with done bands on bottom,
							// need to fix when done
							int qt = 1;
							if (row != j)
								qt = -1;
							exec(0, 0, qt);
							exec(4, j, -1);
							exec(0, 0, -qt);
							break;
						}
					}
				}
			}

			// get stickers from right
			// first turn Front face clockwise so it is parallel to right
			exec(0, 0, 1);
			// update stickers in band
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][j][cubeWidth - 1 - i];
			}

			for (int j = 1; j < N - 1; ++j) { // find appropriate missing stickers from right
				if (band[j] != color) {
					int row = j;
					int col = N - 1 - i;
					std::vector<glm::ivec2> possible;
					getPossiblePositions(row, col, possible);
					for (size_t k = 0; k < possible.size(); ++k) {
						glm::ivec2 temp = possible[k];
						int r = temp[0];
						int c = temp[1];

						// if the right face, at those possible 4 locations, has the color desired,
						if (faces[1][r][c] == color) {
							// if k > 0, may need to rotate Left face to get the sticker in desired position before sticking in band
							exec(1, 0, -k);

							// rotate sticker into band
							exec(2, j, 1);

							// might have pushed some color from front to left
							// so first rotate front, then rotate the above back
							int qt = 1;
							if (N - 1 - col != j) {
								qt = -1;
							}
							exec(0, 0, qt);
							exec(2, j, -1);
							exec(0, 0, -qt);

							break; // we only need one sticker in this position
						}
					}
				}
			}

			// update stickers in band
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][j][cubeWidth - 1 - i];
			}

			// get stickers from back
			for (int j = 1; j < N - 1; ++j) { // find appropriate missing stickers
				if (band[j] != color) {
					int row = j;
					int col = N - 1 - i;
					std::vector<glm::ivec2> possible;
					getPossiblePositions(row, col, possible);
					for (size_t k = 0; k < possible.size(); ++k) {
						glm::ivec2 temp = possible[k];
						int r = temp[0];
						int c = temp[1];

						// if the right face, at those possible 4 locations, has the color desired,
						if (faces[5][r][c] == color) {
							// if k > 0, may need to rotate Left face to get the sticker in desired position before sticking in band
							exec(5, 0, -k);

							// rotate sticker into band
							exec(2, j, 2);

							// might have pushed some color front front to back
							// so first rotate front, then rotate the above back
							int qt = 1;
							if (N - 1 - col != j) {
								qt = -1;
							}
							exec(0, 0, qt);
							exec(2, j, -2);
							exec(0, 0, -qt);

							break; // we only need one sticker in this position
						}
					}
				}
			}

			// update stickers in band
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][j][cubeWidth - 1 - i];
			}

			// turn front face so that band is horizontal, facing bottom
			exec(0, 0, 1);

			// get stickers from bottom; band is still parallel to bottom
			for (int j = 1; j < N - 1; ++j) {
				if (band[j] != color) {
					int row = N - 1 - i;
					int col = N - 1 - j;
					std::vector<glm::ivec2> possible;
					getPossiblePositions(row, col, possible);
					for (size_t k = 0; k < possible.size(); ++k) {
						glm::ivec2 temp = possible[k];
						int r = temp[0];
						int c = temp[1];

						// if the down face, at those possible 4 locations, has the color desired, and is NOT already taken
						if (faces[3][r][c] == color && c >= i) {

							if (k % 2 == 0) { // only rotate if even
								// if k > 0, may need to rotate Down face to get the sticker in desired position before sticking in band
								exec(3, 0, -k);

								// rotate sticker into band
								exec(1, j, 1);

								// above may have pushed some good stickers on front onto top
								int qt = 1;
								if (N - 1 - row != j) {
									qt = -1;
								}

								exec(0, 0, qt);
								exec(1, j, -1); // reverse action from above
								exec(0, 0, -qt);

								// rotate Down face back to where it was
								exec(3, 0, k);
							}
							else { // rotate front face away
								if (i != c) { // if turning front clockwise is valid
									exec(0, 0, 1);

									// rotate sticker into front face
									exec(1, N - 1 - c, 1);

									exec(0, 0, -1);

									// new position of sticker on front face
									int r2 = N - 1 - c;
									int c2 = r;

									// now rotate the desired sticker from front to right
									exec(2, r2, -1);

									exec(0, 0, 1);
									exec(1, N - 1 - c, -1); // reverse above to restore bottom
									exec(0, 0, -1);


									// put band, which is currently facing bottom, to face right
									exec(0, 0, -1);

									// rotate right face until in right position
									// while the row of desired sticker != jth item of band in pos (j, N-1-i)
									while (r2 != j || c2 != N - 1 - i) {
										exec(1, 0, 1);
										int r3 = c2;
										int c3 = N - 1 - r2;
										r2 = r3;
										c2 = c3;
									}

									// move that sticker into the band
									exec(2, j, 1);

									// put band, which is parallel to right, down so it is parallel to down
									exec(0, 0, 1);
								}
								else { // turn front face counterclockwise
									exec(0, 0, -1);

									// rotate sticker into front face
									exec(1, N - 1 - c, 1);

									exec(0, 0, 1);

									// new position of sticker on front face
									int r2 = c;
									int c2 = N - 1 - r;

									// now rotate the desired sticker from front to right
									exec(2, r2, -1);

									exec(0, 0, -1);
									exec(1, N - 1 - c, -1);
									exec(0, 0, 1);

									// put band, which is currently facing bottom, to face right
									exec(0, 0, -1);

									// rotate right face until in right position
									// while the row of desired sticker != jth item of band in pos (j, N-1-i)
									while (r2 != j || c2 != N - 1 - i) {
										exec(1, 0, 1);
										int r3 = c2;
										int c3 = N - 1 - r2;
										r2 = r3;
										c2 = c3;
									}

									// move that sticker into the band
									exec(2, j, 1);

									// put band, which is parallel to right, down so it is parallel to down
									exec(0, 0, 1);

								}
							}


							break; // we only need one sticker in this position
						}
					}
				}
			}

			// update stickers in band; band still parallel to down face
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
			}

			// get stickers from front AGAIN
			for (int j = 1; j < N - 1; ++j) {
				if (band[j] != color) {
					int row = N - 1 - i;
					int col = N - 1 - j;
					std::vector<glm::ivec2> possible;
					getPossiblePositions(row, col, possible);
					for (size_t k = 0; k < possible.size(); ++k) {
						glm::ivec2 temp = possible[k];
						int r = temp[0];
						int c = temp[1];

						// if the down face, at those possible 4 locations, has the color desired,
						// and is not in same row as the band is currently in
						if (faces[0][r][c] == color && r != row) {
							// current position of sticker on front face
							int r2 = r;
							int c2 = c;

							// now rotate the desired sticker from front to right
							exec(2, r2, -1);

							// put band, which is currently facing bottom, to face right
							exec(0, 0, -1);

							// rotate right face until in right position
							// while the row of desired sticker != jth item of band in pos (j, N-1-i)
							while (r2 != j || c2 != N - 1 - i) {
								exec(1, 0, 1);
								int r3 = c2;
								int c3 = N - 1 - r2;
								r2 = r3;
								c2 = c3;
							}

							// move that sticker into the band
							exec(2, j, 1);

							// put band, which is parallel to right, down so it is parallel to down
							exec(0, 0, 1);

							break; // we only need one sticker
						}
					}
				}
			}

			completed = true; 
			// update stickers in band; band still parallel to down face
			for (int j = 0; j < N; ++j) {
				band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
				if (0 < j && j < N-1 && band[j] != color) {
					completed = false;
				}
			}


		} // end while; band parallel to down face

		// orient band back to original; parallel with left
		exec(0, 0, 1);

		// put band/layer back in bottom face
		exec(4, i, 1);
	}
}

// solve Up face of cube after solving Down face (on even sized cubes, it is white)
void Solver::solveCenter1() {
	int color = getFaceColor(2); // get the desired color of Up face of cube
	int NUM = 1;
	if (cubeWidth % 2 == 1)
		NUM = 2;
	for (int numTimes = 0; numTimes < NUM; ++numTimes) {

		for (int i = 1; i < N - 1; ++i) {

			// special case for middle column; skip first and handle later
			if (N % 2 == 1 && i == N / 2) {
				continue;
			}

			// handle the corner case where you accidentally put the piece you need back in Up
			if (numTimes == 1 && faces[0][N / 2][N - 1 - i] == color) {
				exec(0, 0, 1);
			}

			int band[cubeWidth];
			for (int j = 0; j < N; ++j) {
				band[j] = faces[2][N - 1 - j][i];
			}

			// bring this band/layer down onto Front face
			exec(4, i, 1);

			// turn front face so band in opposite (facing right)
			exec(0, 0, 2);

			// undo the above to preserve down face
			exec(4, i, -1);

			// turn band so it is parallel to down
			exec(0, 0, 1);
			
			bool completed = false;
			while (!completed) {

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
				}

				// get stickers from front
				for (int j = 1; j < N - 1; ++j) {
					if (band[j] != color) {
						int row = N - 1 - i;
						int col = N - 1 - j;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the front face, at those possible 4 locations, has the color desired,
							// and is not in same row as the band is currently in
							if (faces[0][r][c] == color && r != row) {
								// current position of sticker on front face
								int r2 = r;
								int c2 = c;

								// now rotate the desired sticker from front to right
								exec(2, r2, -1);

								// need to undo above
								int qt = 1;
								if (N - 1 - c2 != r2) {
									qt = -1;
								}

								exec(1, 0, qt); // first rotate right face out of the way
								exec(2, r2, 1); // undo above
								exec(1, 0, -qt); // undo getting out of way


								// rotate right face until in right position
								// while the row of desired sticker != jth item of band in pos (j, N-1-i)
								while (r2 != j || c2 != N - 1 - i) {
									exec(1, 0, 1);
									int r3 = c2;
									int c3 = N - 1 - r2;
									r2 = r3;
									c2 = c3;
								}

								// put band, which is currently facing bottom, to face right
								exec(0, 0, -1);

								// move that sticker into the band
								exec(2, j, 1);

								// put band, which is parallel to right, down so it is parallel to down
								exec(0, 0, 1);

								break; // we only need one sticker
							}
						}
					}
				}

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
				}

				// rotate band until parallel to left
				exec(0, 0, 1);

				// get appropriate stickers from Left face
				for (int j = 1; j < N - 1; ++j) { // find appropriate missing stickers
					if (band[j] != color) {
						int row = N - 1 - j;
						int col = i;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the left face, at those possible 4 locations, has the color desired,
							if (faces[4][r][c] == color) {
								// if k > 0, may need to rotate Left face to get the sticker in desired position before sticking in band
								exec(4, 0, -k);

								// rotate sticker into band
								exec(2, row, -1);

								int qt = 1;
								if (N - 1 - col != row) {
									qt = -1;
								}

								// first rotate front before
								exec(0, 0, qt);
								// turning back the above above rotation
								exec(2, row, 1);
								exec(0, 0, -qt);

								break; // we only need one sticker in this position
							}
						}
					}
				}

				// first turn front face; now parallel to up face
				exec(0, 0, 1);
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][i][j];
				}

				// get stickers from Up face, if numTime == 0
				if (numTimes == 0) {

					for (int j = 1; j < N - 1; ++j) {
						if (band[j] != color) {
							int row = i;
							int col = j;
							std::vector<glm::ivec2> possible;
							getPossiblePositions(row, col, possible);
							for (size_t k = 0; k < possible.size(); ++k) {
								glm::ivec2 temp = possible[k];
								int r = temp[0];
								int c = temp[1];

								// if we have desired sticker on Up face,
								// if the down face, at those possible 4 locations, has the color desired, and is NOT already taken
								if (faces[2][r][c] == color && c >= i) {

									if (k % 2 == 0) { // only rotate if even
										// if k > 0, may need to rotate Up face to get the sticker in desired position before sticking in band
										exec(2, 0, -k);

										// rotate sticker into band
										exec(1, N - 1 - j, -1);

										// above may have pushed some good stickers on front onto bottom
										int qt = 1;
										if (row != j) {
											qt = -1;
										}

										exec(0, 0, qt);
										exec(1, N - 1 - j, 1); // reverse action from above
										exec(0, 0, -qt);

										// rotate Up face back to where it was
										exec(2, 0, k);
									}
									else { // rotate front face away
										if (i != c) { // if turning front counterclockwise is valid
											exec(0, 0, -1);

											// first rotate a yellow band from Down to Front
											// then when we rotate desired sticker into Front
											// we bring yellow band back down
											// but first move desired sticker on Up face out of the way
											int qt = 2;
											if (r != c) {
												//	qt = -1;
											}

											exec(2, 0, qt);
											exec(1, N - 1 - c, 1); // bring Yellow band up for the ride down
											exec(2, 0, -qt);

											// rotate sticker into front face; as well as bring yellow band back down
											exec(1, N - 1 - c, -1);

											exec(0, 0, 1); // now parallel to Up

											// new position of sticker on front face
											int r2 = c;
											int c2 = N - 1 - r;

											// now rotate the desired sticker from front to right
											exec(2, r2, -1);

											// undo everything above
											exec(0, 0, -1); // parallel to left
											exec(1, N - 1 - c, 1); // reverse above to restore Up face
											exec(2, 0, qt);
											exec(1, N - 1 - c, -1);
											exec(2, 0, -qt);

											exec(0, 0, 1); // parallel to Up

											// put band, which is currently facing up, to face right
											exec(0, 0, 1);

											// rotate right face until in right position
											// while the row of desired sticker != jth item of band in pos (j, N-1-i)
											while (r2 != j || c2 != N - 1 - i) {
												exec(1, 0, 1);
												int r3 = c2;
												int c3 = N - 1 - r2;
												r2 = r3;
												c2 = c3;
											}

											// move that sticker into the band
											exec(2, j, 1);

											// put band, which is parallel to right, down so it is parallel to up
											exec(0, 0, -1);
										}
										else { // turn front face clockwise
											exec(0, 0, 1); // now parallel to right

											// first rotate a yellow band from Down to Front
											// then when we rotate desired sticker into Front
											// we bring yellow band back down
											// but first move desired sticker on Up face out of the way
											int qt = 2;
											if (r != c) {
												//	qt = -1;
											}

											exec(2, 0, qt);
											exec(1, N - 1 - c, 1); // bring Yellow band up for the ride down
											exec(2, 0, -qt);

											// rotate sticker into front face
											exec(1, N - 1 - c, -1);

											exec(0, 0, -1); // now parallel to Up

											// new position of sticker on front face
											int r2 = N - 1 - c;
											int c2 = r;

											// now rotate the desired sticker from front to right
											exec(2, r2, -1);

											// undo everything above
											exec(0, 0, 1); // parallel to right
											exec(1, N - 1 - c, 1); // bring white band to back
											exec(2, 0, qt);
											exec(1, N - 1 - c, -1); // put white band in correct place
											exec(2, 0, -qt);

											exec(0, 0, -1); // now parallel to Up


											// put band, which is currently facing up, to face right
											exec(0, 0, 1);

											// rotate right face until in right position
											// while the row of desired sticker != jth item of band in pos (j, N-1-i)
											while (r2 != j || c2 != N - 1 - i) {
												exec(1, 0, 1);
												int r3 = c2;
												int c3 = N - 1 - r2;
												r2 = r3;
												c2 = c3;
											}

											// move that sticker into the band
											exec(2, j, 1);

											// put band, which is parallel to right, down so it is parallel to up
											exec(0, 0, -1);

										}
									}


									break; // we only need one sticker in this position
								}
							}
						}
					} // band currently parallel to up
				}

				// update band; parallel to up
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][i][j];
				}

				// first turn Front face clockwise so band is parallel to right
				exec(0, 0, 1);

				// find appropriate missing stickers from right
				for (int j = 1; j < N - 1; ++j) {
					if (band[j] != color) {
						int row = j;
						int col = N - 1 - i;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the right face, at those possible 4 locations, has the color desired,
							if (faces[1][r][c] == color) {
								// if k > 0, may need to rotate Left face to get the sticker in desired position before sticking in band
								exec(1, 0, -k);

								// rotate sticker into band
								exec(2, j, 1);

								// might have pushed some color from front to left
								// so first rotate front, then rotate the above back
								int qt = 1;
								if (N - 1 - col != j) {
									qt = -1;
								}
								exec(0, 0, qt);
								exec(2, j, -1);
								exec(0, 0, -qt);

								break; // we only need one sticker in this position
							}
						}
					}
				}

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][j][cubeWidth - 1 - i];
				}

				// get stickers from back
				for (int j = 1; j < N - 1; ++j) {
					if (band[j] != color) {
						int row = j;
						int col = N - 1 - i;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the right face, at those possible 4 locations, has the color desired,
							if (faces[5][r][c] == color) {
								// if k > 0, may need to rotate Left face to get the sticker in desired position before sticking in band
								exec(5, 0, -k);

								// rotate sticker into band
								exec(2, j, 2);

								// might have pushed some color front front to back
								// so first rotate front, then rotate the above back
								int qt = 1;
								if (N - 1 - col != j) {
									qt = -1;
								}
								exec(0, 0, qt);
								exec(2, j, -2);
								exec(0, 0, -qt);

								break; // we only need one sticker in this position
							}
						}
					}
				}

				// band currently facing right

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][j][cubeWidth - 1 - i];
				}

				// make band parallel to down face
				exec(0, 0, 1); 

				completed = true;
				// update stickers in band; band still parallel to down face
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
					if (0 < j && j < N - 1 && band[j] != color) {
						completed = false;
					}
				}

				// band still parallel to down 
			} // end while; band parallel to down face

			// orient band back to original; parallel to left
			exec(0, 0, 1);

			// putting band directly up into up face will disturb down face
			// so we will first move the down band first after moving current band out of the way
			exec(0, 0, 2);
			exec(4, i, 1);
			exec(0, 0, 2);
			// now both bands are offset by one

			// put band back in up face; fixing both bands
			exec(4, i, -1);

		} // end for loop with i

		// if numTimes == 0 and N is odd, then rotate Up face once, so that middle unsolved column becomes middle unsolved row
		if(NUM == 2)
			exec(2, 0, 1);

	} // end for loop with numTimes
}

// solve Back face of cube after solving Down and Up face (on even sized cubes, this back color is blue)
void Solver::solveCenter2() {
	int color = getFaceColor(5);

	int NUM = 1;
	if (cubeWidth % 2 == 1)
		NUM = 2;

	// pull same trick of solving every row but middle row (if N odd)
	// then turn sideways so that blue face is solved except for middle column
	// then resolve rows again, to get full solved center

	for (int numTimes = 0; numTimes < NUM; ++numTimes) {

		for (int i = 1; i < N - 1; ++i) {
			
			if (N % 2 == 1 && i == N / 2) {
				continue; // don't solve middle column explicitly
			}

			// handle the corner case where you accidentally put the piece you need back in Back
			if (numTimes == 1 && faces[0][N - 1 - i][N / 2] == color) {
				exec(0, 0, 1);
			}

			// doing row by row of back face
			// bring band from back to front
			exec(2, i, 2); // parallel to up

			// have band parallel to down
			exec(0, 0, 2);

			int band[cubeWidth];

			bool completed = false;
			while (!completed) {

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
				}

				// get stickers from front
				for (int j = 1; j < N - 1; ++j) {
					if (band[j] != color) {
						int row = N - 1 - i;
						int col = N - 1 - j;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the front face, at those possible 4 locations, has the color desired,
							// and is not in same row as the band is currently in
							if (faces[0][r][c] == color && r != row) {
								// current position of sticker on front face
								int r2 = r;
								int c2 = c;

								// now rotate the desired sticker from front to right
								exec(2, r2, -1);

								// need to undo above
								int qt = 1;
								if (N - 1 - c2 != r2) {
									qt = -1;
								}

								exec(1, 0, qt); // first rotate right face out of the way
								exec(2, r2, 1); // undo above
								exec(1, 0, -qt); // undo getting out of way


								// rotate right face until in right position
								// while the row of desired sticker != jth item of band in pos (j, N-1-i)
								while (r2 != j || c2 != N - 1 - i) {
									exec(1, 0, 1);
									int r3 = c2;
									int c3 = N - 1 - r2;
									r2 = r3;
									c2 = c3;
								}

								// put band, which is currently facing bottom, to face right
								exec(0, 0, -1);

								// move that sticker into the band
								exec(2, j, 1);

								// put band, which is parallel to right, down so it is parallel to down
								exec(0, 0, 1);

								// undo above
								exec(2, j, -1);

								break; // we only need one sticker
							}
						}
					}
				}

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
				}

				// rotate band from down until parallel to left
				exec(0, 0, 1);

				// get appropriate stickers from Left face
				for (int j = 1; j < N - 1; ++j) { // find appropriate missing stickers
					if (band[j] != color) {
						int row = N - 1 - j;
						int col = i;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the left face, at those possible 4 locations, has the color desired,
							if (faces[4][r][c] == color) {
								// if k > 0, may need to rotate Left face to get the sticker in desired position before sticking in band
								exec(4, 0, -k);

								// rotate sticker into band
								exec(2, row, -1);

								int qt = 1;
								if (N - 1 - col != row) {
									qt = -1;
								}

								// first rotate front before
								exec(0, 0, qt);
								// turning back the above above rotation
								exec(2, row, 1);
								exec(0, 0, -qt);

								break; // we only need one sticker in this position
							}
						}
					}
				}

				// make band from left until parallel to right face
				exec(0, 0, 2);

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][j][cubeWidth - 1 - i];
				}

				// find appropriate missing stickers from right
				for (int j = 1; j < N - 1; ++j) {
					if (band[j] != color) {
						int row = j;
						int col = N - 1 - i;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the right face, at those possible 4 locations, has the color desired,
							if (faces[1][r][c] == color) {
								// if k > 0, may need to rotate Right face to get the sticker in desired position before sticking in band
								exec(1, 0, -k);

								// rotate sticker into band
								exec(2, j, 1);

								// might have messed with solved rows on back
								// so first rotate front, then rotate the above back
								int qt = 1;
								if (N - 1 - col != j) {
									qt = -1;
								}
								exec(0, 0, qt);
								exec(2, j, -1);
								exec(0, 0, -qt);

								break; // we only need one sticker in this position
							}
						}
					}
				}

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][j][cubeWidth - 1 - i];
				}

				// find appropriate missing stickers from back; band parallel to right; only if first time
				if (numTimes == 0) {
					for (int j = 1; j < N - 1; ++j) {
						if (band[j] != color) {
							int row = j;
							int col = N - 1 - i;
							std::vector<glm::ivec2> possible;
							getPossiblePositions(row, col, possible);
							for (size_t k = 0; k < possible.size(); ++k) {
								glm::ivec2 temp = possible[k];
								int r = temp[0];
								int c = temp[1];

								// if the back face, at those possible 4 locations, has the color desired,
								// and it is not already used
								if (faces[5][r][c] == color && r >= i) {
									int qt = 1;
									if (N - 1 - col != r) {
										qt = -1;
									}

									exec(0, 0, qt);
									// first bring sticker into right face
									exec(2, r, 1);
									exec(0, 0, -qt); // band parallel to right again

									// if k > 0, may need to rotate Right face to get the sticker in desired position before sticking in band
									exec(1, 0, -k);

									// rotate sticker into band
									exec(2, j, 1);

									// might have messed with solved rows on back
									// so first rotate front, then rotate the above back
									qt = 1;
									if (N - 1 - col != j) {
										qt = -1;
									}
									exec(0, 0, qt);
									exec(2, j, -1);
									exec(0, 0, -qt);

									break; // we only need one sticker in this position
								}
							}
						}
					}
				}

				// make band from right until parallel to down face
				exec(0, 0, 1);

				completed = true;
				// update stickers in band; band still parallel to down face
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
					if (0 < j && j < N - 1 && band[j] != color) {
						completed = false;
					}
				}
			} // end while

			// orient band back to original; from down until parallel to up
			exec(0, 0, 2);

			// move band from front to back
			exec(2, i, 2);

		} // end for loop with i

		// if numTimes == 0 and N is odd, then rotate Back face once, so that middle unsolved row becomes middle unsolved col
		if(NUM == 2)
			exec(5, 0, 1);

	} // end for loop with numTimes

}

// solve Left face of cube after solving Down, Up, Back face (on even sized cubes, this left color is red)
void Solver::solveCenter3() {
	int color = getFaceColor(4);

	int NUM = 1;
	if (cubeWidth % 2 == 1)
		NUM = 2;

	// pull same trick of solving every row but middle row (if N odd)
	// then turn sideways so that blue face is solved except for middle column
	// then resolve rows again, to get full solved center

	for (int numTimes = 0; numTimes < NUM; ++numTimes) {

		for (int i = 1; i < N - 1; ++i) {

			if (N % 2 == 1 && i == N / 2) {
				continue; // don't solve middle column explicitly
			}

			// handle the corner case where you accidentally put the piece you need back in Left
			if (numTimes == 1 && faces[0][N - 1 - i][N / 2] == color) {
				exec(0, 0, 1);
			}

			// doing row by row of left face
			// bring band from left to front
			exec(2, i, -1); // parallel to up

			// have band parallel to down
			exec(0, 0, 2);

			// undo above to preserve solved back face
			exec(2, i, 1);

			int band[cubeWidth];

			bool completed = false;
			while (!completed) { // band parallel to down face

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
				}

				// get stickers from front
				for (int j = 1; j < N - 1; ++j) {
					if (band[j] != color) {
						int row = N - 1 - i;
						int col = N - 1 - j;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the front face, at those possible 4 locations, has the color desired,
							// and is not in same row as the band is currently in
							if (faces[0][r][c] == color && r != row) {
								// current position of sticker on front face
								int r2 = r;
								int c2 = c;

								// now rotate the desired sticker from front to right
								exec(2, r2, -1);

								// need to undo above
								int qt = 1;
								if (N - 1 - c2 != r2) {
									qt = -1;
								}

								exec(1, 0, qt); // first rotate right face out of the way
								exec(2, r2, 1); // undo above
								exec(1, 0, -qt); // undo getting out of way


								// rotate right face until in right position
								// while the row of desired sticker != jth item of band in pos (j, N-1-i)
								while (r2 != j || c2 != N - 1 - i) {
									exec(1, 0, 1);
									int r3 = c2;
									int c3 = N - 1 - r2;
									r2 = r3;
									c2 = c3;
								}

								// put band, which is currently facing bottom, to face right
								exec(0, 0, -1);

								// move that sticker into the band
								exec(2, j, 1);

								// put band, which is parallel to right, down so it is parallel to down
								exec(0, 0, 1);

								// undo above
								exec(2, j, -1);

								break; // we only need one sticker
							}
						}
					}
				}

				// rotate band from down to parallel to right
				exec(0, 0, -1);

				// update stickers in band
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][j][cubeWidth - 1 - i];
				}

				// find appropriate missing stickers from right
				for (int j = 1; j < N - 1; ++j) {
					if (band[j] != color) {
						int row = j;
						int col = N - 1 - i;
						std::vector<glm::ivec2> possible;
						getPossiblePositions(row, col, possible);
						for (size_t k = 0; k < possible.size(); ++k) {
							glm::ivec2 temp = possible[k];
							int r = temp[0];
							int c = temp[1];

							// if the right face, at those possible 4 locations, has the color desired,
							if (faces[1][r][c] == color) {
								// if k > 0, may need to rotate Right face to get the sticker in desired position before sticking in band
								exec(1, 0, -k);

								// rotate sticker into band
								exec(2, j, 1);

								// might have messed with solved rows on back
								// so first rotate front, then rotate the above back
								int qt = 1;
								if (N - 1 - col != j) {
									qt = -1;
								}
								exec(0, 0, qt);
								exec(2, j, -1);
								exec(0, 0, -qt);

								break; // we only need one sticker in this position
							}
						}
					}
				}

				// update stickers in band; currently band parallel to right
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][j][cubeWidth - 1 - i];
				}

				// find appropriate missing stickers from left; band parallel to right; only if first time
				if (numTimes == 0) {

					for (int j = 1; j < N - 1; ++j) {
						if (band[j] != color) {
							int row = j;
							int col = N - 1 - i;
							std::vector<glm::ivec2> possible;
							getPossiblePositions(row, col, possible);
							for (size_t k = 0; k < possible.size(); ++k) {
								glm::ivec2 temp = possible[k];
								int r = temp[0];
								int c = temp[1];

								// if the left face, at those possible 4 locations, has the color desired,
								// and it is not already used
								if (faces[4][r][c] == color && r >= i) {
									int qt = 1;
									if (N - 1 - col != r) {
										qt = -1;
									}

									exec(0, 0, qt);
									// first bring sticker into right face
									exec(2, r, -2);

									// to undo above,
									int qt2 = 1;
									if (N - 1 - c != r) {
										qt2 = -1;
									}
									exec(1, 0, qt); // move desired sticker out of the way
									exec(2, r, 2); // undo above
									exec(1, 0, -qt); // move desired sticker back

									exec(0, 0, -qt); // band parallel to right again

									// if k > 0, may need to rotate Right face to get the sticker in desired position before sticking in band
									exec(1, 0, -k);

									// rotate sticker into band
									exec(2, j, 1);

									// might have messed with solved rows on back
									// so first rotate front, then rotate the above back
									qt = 1;
									if (N - 1 - col != j) {
										qt = -1;
									}
									exec(0, 0, qt);
									exec(2, j, -1);
									exec(0, 0, -qt); // band parallel to right again

									break; // we only need one sticker in this position
								}
							}
						}
					}
				}

				exec(0, 0, 1); // move band from right to down

				completed = true;
				// update stickers in band; band still parallel to down face
				for (int j = 0; j < N; ++j) {
					band[j] = faces[0][cubeWidth - 1 - i][cubeWidth - 1 - j];
					if (0 < j && j < N - 1 && band[j] != color) {
						completed = false;
					}
				}

			} // end while, band parallel to down

			// to move band back to left, need to have back band moved as well
			exec(2, i, -1);

			// orient band back to original row; from down until parallel to up
			exec(0, 0, 2);

			// move band from front to left, fixing both current band and back face at once
			exec(2, i, 1);

		} // end for loop with i

		// if numTimes == 0 and N is odd, then rotate Back face once, so that middle unsolved row becomes middle unsolved col
		if (NUM == 2)
			exec(4, 0, 1);

	} // end for loop with numTimes
}

// solve Right and Front face of cube after solving Down, Up, Back, Left face (on even sized cubes, this right = red, front = green)
// https://www.youtube.com/watch?v=4UnzSSNxcRc - JRCuber
// I personally take no credit for being able to solve the last two centers, tho I did learn something new
void Solver::solveLastCenters() {
	int colorF = getFaceColor(0);
	int colorR = getFaceColor(1);

	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			for (int j = 1; j < N - 1; ++j) {
				// if two stickers in the same row,col on both faces need to be swapped
				if (faces[0][i][j] == colorR && faces[1][i][j] == colorF) {
					// swapping algorithm learned in the video by JRCuber
					exec(2, i, -1); // replace sticker on right face

					int qt = -1;
					int r2 = N - 1 - j;
					int c2 = i;

					if (i != j) {
						qt = 1;
						r2 = j;
						c2 = N - 1 - i;
					}
					exec(1, 0, qt); // move right face out of the way
					exec(2, r2, -1); // intersect it with another layer to the right
					exec(1, 0, -qt); // reverse the moving out of the way

					exec(2, i, 1); // undo bringing the first layer in
					exec(1, 0, qt); 
					exec(2, r2, 1); // undo bringing second layer in
					exec(1, 0, -qt);
					
					// other faces untouched; stickers are swapped
				}
			}
		}

		exec(1, 0, 1); // repeat four times, with turning right face between each 4
	}	
}

// Credits to https://ruwix.com/twisty-puzzles/big-cubes-nxnxn-solution/ 
// flips the edge between front and right
void Solver::flipEdge(int edge) {
	numEdgeFlip++;
	if (edge == 0 + 1) { // front + right
		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(0, 0, 1);
		exec(1, 0, -1);
		exec(0, 0, -1);
		exec(1, 0, 1);
	}
	else if (edge == 1 + 5) { // right + back
		exec(5, 0, 1);
		exec(2, 0, 1);
		exec(5, 0, -1);
		exec(1, 0, 1);
		exec(5, 0, -1);
		exec(1, 0, -1);
		exec(5, 0, 1);
	}
	else if (edge == 5 + 4) { // back + left
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(4, 0, -1);
		exec(5, 0, 1);
		exec(4, 0, -1);
		exec(5, 0, -1);
		exec(4, 0, 1);
	}
	else if (edge == 4 + 0) { // left + front
		exec(0, 0, 1);
		exec(2, 0, 1);
		exec(0, 0, -1);
		exec(4, 0, 1);

	//	std::cout << "MIDWAY :" << std::endl;
	//	print();

		exec(0, 0, -1);

	//	std::cout << "5/7: " << std::endl;
	//	print();

		exec(4, 0, -1);

		//std::cout << "3/4 of the way:" << std::endl;
		//print();

		exec(0, 0, 1);
	}
}

void Solver::solveEdges0(std::vector<std::pair<int, int>>& colorPairs) {
	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';

	// build edge on edge between Front and Right face
	for(int numTimes = 0; numTimes < 4; ++numTimes) {
		int colorF = colorPairs[numTimes].first;
		int colorR = colorPairs[numTimes].second;

		std::cout << "Edges 0-4 doing " << myMap[colorF] << " " << myMap[colorR] << std::endl;

		for (int i = 1; i < N - 1; ++i) {
			if (faces[0][i][N - 1] == colorF && faces[1][i][0] == colorR) {
				continue;
			}
			
			// edge between front and up
			if (faces[0][0][i] == colorF && faces[2][N - 1][i] == colorR) {
				exec(2, i, 1);
				exec(4, 0, -1);

				exec(2, 0, 1);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[2][N - 1][N - 1 - i] == colorF && faces[0][0][N - 1 - i] == colorR) {
				exec(2, i, 2);
				exec(4, 0, 1);

				exec(2, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// edge between left and up
			else if (faces[4][0][i] == colorF && faces[2][i][0] == colorR) {
				exec(2, 0, 1);
				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);

				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[2][N - 1 - i][0] == colorF && faces[4][0][N - 1 - i] == colorR) {
				exec(2, 0, 1);
				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// edge between back and up
			else if (faces[5][0][i] == colorF && faces[2][0][N - 1 - i] == colorR) {
				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[2][0][i] == colorF && faces[5][0][N - 1 - i] == colorR) {
				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// edge between right and up
			else if (faces[1][0][i] == colorF && faces[2][N - 1 - i][N - 1] == colorR) {
				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, 2);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[2][i][N - 1] == colorF && faces[1][0][N - 1 - i] == colorR) {
				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, 2);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// edge between front and left
			else if (faces[0][N - 1 - i][0] == colorF && faces[4][N - 1 - i][N - 1] == colorR) {
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// other one
			else if (faces[4][i][N - 1] == colorF && faces[0][i][0] == colorR) {
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, 1);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// edge between left and back
			else if (faces[4][N - 1 - i][0] == colorF && faces[5][N - 1 - i][N - 1] == colorR) {
				exec(5, 0, -1);
				exec(2, 0, 1);
				exec(5, 0, 1);
				exec(2, 0, -1);

				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// other one
			else if (faces[5][i][N - 1] == colorF && faces[4][i][0] == colorR) {
				exec(5, 0, -1);
				exec(2, 0, 1);
				exec(5, 0, 1);
				exec(2, 0, -1);

				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// edge between right and back
			else if (faces[1][i][N - 1] == colorF && faces[5][i][0] == colorR) {
				exec(5, 0, 1);
				exec(2, 0, -1);
				exec(5, 0, -1);
				exec(2, 0, 1);
				
				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// other one
			else if (faces[5][N - 1 - i][0] == colorF && faces[1][N - 1 - i][N - 1] == colorR) {
				exec(5, 0, 1);
				exec(2, 0, -1);
				exec(5, 0, -1);
				exec(2, 0, 1);

				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// edge between down and front
			else if (faces[0][N - 1][N - 1 - i] == colorF && faces[3][0][N - 1 - i] == colorR) {
				exec(0, 0, 2);
				exec(2, 0, -1);
				exec(0, 0, -2);

				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, 2);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[3][0][i] == colorF && faces[0][N - 1][i] == colorR) {
				exec(0, 0, 2);
				exec(2, 0, -1);
				exec(0, 0, -2);

				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, 2);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// edge between down and left
			else if (faces[4][N - 1][N - 1 - i] == colorF && faces[3][i][0] == colorR) {
				exec(4, 0, 2);
				exec(2, 0, -1);
				exec(4, 0, -2);

				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, 1);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[3][N - 1 - i][0] == colorF && faces[4][N - 1][i] == colorR) {
				exec(4, 0, 2);
				exec(2, 0, -1);
				exec(4, 0, -2);

				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// edge between down and back
			else if (faces[5][N - 1][N - 1 - i] == colorF && faces[3][N - 1][i] == colorR) {
				exec(5, 0, 2);
				exec(2, 0, 1);
				exec(5, 0, -2);

				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, 2);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[3][N - 1][N - 1 - i] == colorF && faces[5][N - 1][i] == colorR) {
				exec(5, 0, 2);
				exec(2, 0, 1);
				exec(5, 0, -2);

				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, 2);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// edge between down and right
			else if (faces[1][N - 1][N - 1 - i] == colorF && faces[3][N - 1 - i][N - 1] == colorR) {
				exec(1, 0, 2);
				exec(2, 0, 1);
				exec(1, 0, -2);

				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, 0, 1);
				exec(4, 0, 1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[3][i][N - 1] == colorF && faces[1][N - 1][i] == colorR) {
				exec(1, 0, 2);
				exec(2, 0, 1);
				exec(1, 0, -2);

				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
			// other edge piece on edge between front and right
			else if (faces[1][N - 1 - i][0] == colorF && faces[0][N - 1 - i][N - 1] == colorR) {
				exec(2, N - 1 - i, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				exec(2, N - 1 - i, -1);

				exec(2, i, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
			}
		}

		exec(0, 0, 1);
		exec(3, 0, 1);
	}

	exec(0, 0, 2);
	exec(1, 0, 2);
	exec(4, 0, 2);
	exec(5, 0, 2);
}

void Solver::solveEdges1(std::vector<std::pair<int, int>>& colorPairs) {

	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';

	// build edge on edge between Front and Right face
	// top edges are all solved
	for (int numTimes = 4; numTimes < 8; ++numTimes) {
		int colorF = colorPairs[numTimes].first;
		int colorR = colorPairs[numTimes].second;

		std::cout << "FR = " << myMap[colorF] << " " << myMap[colorR] << std::endl;

		for (int i = 1; i < N - 1; ++i) {
			if (faces[0][i][N - 1] == colorF && faces[1][i][0] == colorR) {
				continue;
			}

			//std::cout << "i = " << i << std::endl;

			// edge between front and left
			if (faces[0][N - 1 - i][0] == colorF && faces[4][N - 1 - i][N - 1] == colorR) {
				//std::cout << "opt1 " << std::endl;
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -1);
				exec(5, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, 1);
				exec(2, i, 1);

				exec(1, 0, 1);
				exec(3, 0, 1);
				exec(1, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				exec(3, 0, 2);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			// other one
			else if (faces[4][i][N - 1] == colorF && faces[0][i][0] == colorR) {
				//std::cout << "opt2 " << std::endl;
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				exec(2, i, 2);
				
				exec(1, 0, 1);
				exec(3, 0, 1);
				exec(1, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				exec(3, 0, 2);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			// edge between left and back
			else if (faces[4][N - 1 - i][0] == colorF && faces[5][N - 1 - i][N - 1] == colorR) {
				//std::cout << "left + back " << std::endl;
				exec(3, 0, 2);
				exec(5, 0, 1);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(3, 0, 1);
				exec(4, 0, -1);

				// now do front + left
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -1);
				exec(5, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, 1);
				exec(2, i, 1);
				
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			// other one
			else if (faces[5][i][N - 1] == colorF && faces[4][i][0] == colorR) {
				//std::cout << "left + back part 2" << std::endl;
				exec(3, 0, 2);
				exec(5, 0, 1);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(3, 0, 1);
				exec(4, 0, -1);

				// now do front + left
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				exec(2, i, 2);
				
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			
			// edge between right and back
			else if (faces[1][i][N - 1] == colorF && faces[5][i][0] == colorR) {
				//std::cout << "right + back" << std::endl;
				exec(5, 0, -1);
				exec(3, 0, -1);
				exec(5, 0, 1);
				exec(2, i, 2);
				exec(4, 0, -1);
				exec(3, 0, 2);
				exec(4, 0, 1);
				exec(2, i, -2);

				exec(0, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				exec(3, 0, 1);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(0, 0, 1);
				exec(3, 0, 2);
			}
			// other one
			else if (faces[5][N - 1 - i][0] == colorF && faces[1][N - 1 - i][N - 1] == colorR) {
				//std::cout << "right + back part 2" << std::endl;
				exec(5, 0, -1);
				exec(3, 0, -1);
				exec(5, 0, 1);
				exec(2, i, 1);
				exec(4, 0, 1);
				exec(3, 0, 2);
				exec(4, 0, -1);
				exec(2, i, -1);

				exec(0, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				exec(3, 0, 1);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(0, 0, 1);
				exec(3, 0, 2);
			}
			// edge between down and front
			else if (faces[0][N - 1][N - 1 - i] == colorF && faces[3][0][N - 1 - i] == colorR) {
				//std::cout << "down + front" << std::endl;
				exec(2, i, 1);
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				exec(2, i, -1);
				
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
			}
			// other one
			else if (faces[3][0][i] == colorF && faces[0][N - 1][i] == colorR) {
				//std::cout << "down + front part 2" << std::endl;
				exec(2, i, 2);
				exec(4, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				exec(2, i, -2);
				
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
			}
			
			// edge between down and left
			else if (faces[4][N - 1][N - 1 - i] == colorF && faces[3][i][0] == colorR) {
				//std::cout << "down + left" << std::endl;

				exec(3, 0, 1);

				// do down + front
				exec(2, i, 1);
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				exec(2, i, -1);

				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);

				exec(3, 0, -1);
			}
			// other one
			else if (faces[3][N - 1 - i][0] == colorF && faces[4][N - 1][i] == colorR) {
				//std::cout << "down + left part 2" << std::endl;
				exec(3, 0, 1);

				// do down + front
				exec(2, i, 2);
				exec(4, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				exec(2, i, -2);
				
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);

				exec(3, 0, -1);
			}
			
			// edge between down and back
			else if (faces[5][N - 1][N - 1 - i] == colorF && faces[3][N - 1][i] == colorR) {
				//std::cout << "down + back" << std::endl;
				exec(2, i, 1);
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(3, 0, -1);
				exec(2, i, -1);
			}
			// other one
			else if (faces[3][N - 1][N - 1 - i] == colorF && faces[5][N - 1][i] == colorR) {
				//std::cout << "down + back part 2" << std::endl;
				exec(2, i, 2);
				exec(4, 0, -1);
				exec(3, 0, 1);
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(2, i, -2);
			}
			// edge between down and right
			else if (faces[1][N - 1][N - 1 - i] == colorF && faces[3][N - 1 - i][N - 1] == colorR) {
				//std::cout << "down + right" << std::endl;
				exec(2, i, 2);
				exec(5, 0, 1);
				exec(3, 0, 1);
				exec(5, 0, -1);
				exec(3, 0, -1);
				exec(2, i, -2);
			}
			// other one
			else if (faces[3][i][N - 1] == colorF && faces[1][N - 1][i] == colorR) {
				//std::cout << "down + right part 2" << std::endl;
				exec(2, i, -1);
				exec(5, 0, -1);
				exec(3, 0, 1);
				exec(5, 0, 1);
				exec(3, 0, -1);
				exec(2, i, 1);
			}
			// other edge piece on edge between front and right
			else if (faces[1][N - 1 - i][0] == colorF && faces[0][N - 1 - i][N - 1] == colorR) {
				//std::cout << "reversed: front + right" << std::endl;
				
				exec(2, N - 1 - i, 1);
				if (i != N - 1 - i) {
					exec(2, i, 1);
				}
				exec(4, 0, 2);
				exec(2, i, -2);
				if (i != N - 1 - i) {
					exec(2, N - 1 - i, -2);
				}

				exec(4, 0, -2);

				exec(2, i, 1);
				if (i != N - 1 - i) {
					exec(2, N - 1 - i, 1);
				}

				// now handle using front + left

				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(2, i, -2);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				exec(2, i, 2);

				exec(1, 0, 1);
				exec(3, 0, 1);
				exec(1, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				exec(3, 0, 2);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			
		}
		
		if (numTimes < 7) {
			exec(0, 0, 1);
			exec(3, 0, 1);
			exec(0, 0, -1);
		}		
	}

	
	// top 4 edges are solved, 3 of bottom edges are solved, and 8th solved edge on edge between front and right
	// need to get it so that top 4 edges are solved, bottom 4 edges are solved
	exec(1, 0, -1); // now top = 3, bot = 3, right has 3 (froms U shape)
	exec(3, 0, 1); // now solved top edges and bottom edges are parallel to each other; two vertical edges on right
	exec(1, 0, 1); // place both vertical edges on right into two horizontal edges, one on top, one on bottom -> top has 4, bot has 4
}

void Solver::solveEdges2() {
	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';

	int count = 0;

	// build edge on edge between Front and Right face
	// top edges, bottom edges are all solved
	for (int numTimes = 0; numTimes < 3; numTimes++) {
		int colorF = faces[0][N / 2][N - 1];
		int colorR = faces[1][N / 2][0];

		if (N % 2 == 0) {
			/*
			if (numTimes == 0) {
				colorF = 3;
				colorR = 5;
			}
			else if (numTimes == 1) {
				colorF = 4;
				colorR = 5;
			}
			else if (numTimes == 2) {
				colorF = 4;
				colorR = 3;
			}
			*/
		}

		std::cout << "pursuing colorF = " << myMap[colorF] << " and colorR = " << myMap[colorR] << std::endl;

		for (int i = 1; i < N - 1; ++i) {

			std::cout << "i = " << i << std::endl;
			// correct piece already
			if (faces[0][i][N - 1] == colorF && faces[1][i][0] == colorR) {
				std::cout << "already done" << std::endl;
				//continue;
			}
			// if desired piece on opposite side of same edge
			else if (faces[0][N - 1 - i][N - 1] == colorR && faces[1][N - 1 - i][0] == colorF) {
				std::cout << "on other side of same edge!" << std::endl;
				exec(2, i, -1);
				exec(2, N - 1 - i, -1);
				flipEdge(1 + 5); // right + back
				exec(2, i, 1);
				exec(2, N - 1 - i, 1);

				count += 11;
				std::cout << "now count = " << count << std::endl;
			}
			else if (faces[5][N - 1 - i][0] == colorF && faces[1][N - 1 - i][N - 1] == colorR) {
				std::cout << "edge between right + back, down" << std::endl;
				exec(2, i, -1);
				flipEdge(1 + 5); // right + back
				exec(2, i, 1);

				count += 9;
				std::cout << "now count = " << count << std::endl;
			}
			else if (faces[1][i][N - 1] == colorF && faces[5][i][0] == colorR) {
				std::cout << "edge between right + back, up" << std::endl;
				flipEdge(1 + 5); // right + back
				exec(2, i, -1);
				flipEdge(1 + 5);
				exec(2, i, 1);

				count += 16;
			//	std::cout << "now count = " << count << std::endl;
			}
			else if (faces[0][N - 1 - i][0] == colorF && faces[4][N - 1 - i][N - 1] == colorR) {
				std::cout << "edge between left + front, down" << std::endl;
				exec(2, i, 1);
				flipEdge(4 + 0); // left + front
				exec(2, i, -1);

				count += 9;
			//	std::cout << "now count = " << count << std::endl;
			}
			else if (faces[4][i][N - 1] == colorF && faces[0][i][0] == colorR) {
				std::cout << "edge between left + front, up" << std::endl;
				flipEdge(4 + 0); // left + front
				exec(2, i, 1);
				std::cout << "doing print: " << std::endl;
			//	print();

				flipEdge(4 + 0);

				
			//	std::cout << "STOPPPPPPPPPPPPP" << std::endl;

				exec(2, i, -1);

				count += 16;
		//		std::cout << "now count = " << count << std::endl;
			//	print();
			}
			else if (faces[4][N - 1 - i][0] == colorF && faces[5][N - 1 - i][N - 1] == colorR) {
				std::cout << "edge between back + left, down" << std::endl;
				exec(2, i, 2);
				flipEdge(5 + 4); // back + left
				exec(2, i, -2);

				count += 11;
			//	std::cout << "now count = " << count << std::endl;
			}
			else if (faces[5][i][N - 1] == colorF && faces[4][i][0] == colorR) {
				std::cout << "edge between back + left, up" << std::endl;
				flipEdge(5 + 4);
				exec(2, i, 2);
				flipEdge(5 + 4);
				exec(2, i, -2);

				count += 18;
		//		std::cout << "now count = " << count << std::endl;
			}
		}

	//	std::cout << "here, we are at " << count << std::endl;

		if (numTimes == 0) {
			// bring 1st finished edge to edge between back + left
			exec(1, 0, 2);
			exec(5, 0, 2);

			count += 4;
		//	std::cout << "added 4: " << count << std::endl;
		}
		else if (numTimes == 1) {
			// bring 2nd finished edge to edge between front + left

			exec(0, 0, 2);

			count += 2;
		//	std::cout << "added 2: " << count << std::endl;
		}
		else if (numTimes == 2) {
			// bring 3rd finished edge to edge between right + back
			exec(1, 0, 2);

			count += 2;
		//	std::cout << "added 2 part 2: " << count << std::endl;
		}
	}

	std::cout << "now doing last edge: count = " << count << std::endl;

	// build last edge, on edge between front + right
	int colorF = faces[0][N / 2][N - 1];
	int colorR = faces[1][N / 2][0];

	if (N % 2 == 0) {
	//	colorF = 2;
	//	colorR = 5;
	}

	std::cout << "last edge: colorF = " << myMap[colorF] << " and colorR = " << myMap[colorR] << std::endl;
	for (int i = 1; i < N - 1; ++i) {
		//std::cout << "i2 = " << i << std::endl;
		// correct piece already
		if (faces[0][i][N - 1] == colorF && faces[1][i][0] == colorR) {
			//std::cout << "donezoed" << std::endl;
			continue;
		}
		// if desired piece on opposite side of same edge
		// Red Bull Algorithm - https://www.youtube.com/watch?v=knMCvKdJFgk
		else if (faces[0][N - 1 - i][N - 1] == colorR && faces[1][N - 1 - i][0] == colorF) {
			//std::cout << "on the other side..." << std::endl;
			
			exec(2, N - 1 - i, 2);
			exec(5, 0, 2);
			exec(1, 0, 2);
			exec(2, i, 1);
			exec(1, 0, 2);
			
			exec(2, N - 1 - i, 1);
			exec(1, 0, 2);
			exec(2, N - 1 - i, -1);
			exec(1, 0, 2);
			exec(0, 0, 2);
			exec(2, N - 1 - i, -1);
			exec(0, 0, 2);
			
			exec(2, i, -1);
			exec(5, 0, 2);
			exec(2, N - 1 - i, 2);
		}
	}

}

void Solver::solveEdges() {
	if (N <= 3) {
		return;
	}

	// colors: 0,1,2,3,4,5 -> green, red, white, yellow, orange, blue
	// make all possible pairs of colors that correspond to edges
	std::vector<std::pair<int, int>> colorPairs;
	for (int i = 0; i < 6; ++i) {
		for (int j = i + 1; j < 6; ++j) {
			if (i + j != 5) {
				colorPairs.push_back(std::make_pair(i, j));
			}
		}
	}

	solveEdges0(colorPairs);
	solveEdges1(colorPairs);
	solveEdges2();
}

void Solver::solveCross() {
	// edges only exist for N >= 3
	if (N <= 2) {
		return;
	}

	std::vector<int> sideColors{ getFaceColor(0), getFaceColor(1), getFaceColor(5), getFaceColor(4) };
	
	int colorDown = getFaceColor(3);
	
	// bring each edge onto the edge between front and down, then rotate bottom counterclockwise
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		int colorSide = sideColors[numTimes];

		// already solved
		if (faces[3][0][1] == colorDown && faces[0][N - 1][1] == colorSide) {

		}
		// just backwards
		else if (faces[0][N - 1][1] == colorDown && faces[3][0][1] == colorSide) {
			exec(0, 0, -1);
			exec(1, 0, 1);
			exec(2, 0, 1);
			exec(1, 0, -1);
			exec(0, 0, 2);
		}
		// desired edge on edge between right and down
		else if (faces[3][1][N - 1] == colorDown && faces[1][N - 1][1] == colorSide) {
			exec(1, 0, 2);
			exec(2, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[1][N - 1][1] == colorDown && faces[3][1][N - 1] == colorSide) {
			exec(1, 0, 1);
			exec(0, 0, 1);
		}
		// desired edge on edge between back and down
		else if (faces[3][N - 1][N - 2] == colorDown && faces[5][N - 1][1] == colorSide) {
			exec(5, 0, 2);
			exec(2, 0, 2);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[5][N - 1][1] == colorDown && faces[3][N - 1][N - 2] == colorSide) {
			exec(5, 0, 1);
			exec(1, 0, -1);
			exec(2, 0, 1);
			exec(1, 0, 1);
			exec(0, 0, 2);
		}
		// desired edge on edge between left and down
		else if (faces[3][1][0] == colorDown && faces[4][N - 1][N - 2] == colorSide) {
			exec(3, 0, -1);
		}
		// flipped other way
		else if (faces[4][N - 1][N - 2] == colorDown && faces[3][1][0] == colorSide) {
			exec(4, 0, -1);
			exec(0, 0, -1);
			exec(4, 0, 1);
		}
		// edge between front and right
		else if (faces[1][1][0] == colorDown && faces[0][1][N - 1] == colorSide) {
			exec(0, 0, 1);
		}
		// flipped other way
		else if (faces[0][1][N - 1] == colorDown && faces[1][1][0] == colorSide) {
			exec(1, 0, 1);
			exec(2, 0, 1);
			exec(1, 0, -1);
			exec(0, 0, 2);
		}
		// edge between right and back
		else if (faces[5][1][0] == colorDown && faces[1][1][N - 1] == colorSide) {
			exec(1, 0, -1);
			exec(2, 0, 1);
			exec(1, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[1][1][N - 1] == colorDown && faces[5][1][0] == colorSide) {
			exec(5, 0, 1);
			exec(2, 0, 2);
			exec(5, 0, -1);
			exec(0, 0, 2);
		}
		// edge between back + left
		else if (faces[4][1][0] == colorDown && faces[5][1][N - 1] == colorSide) {
			exec(5, 0, -1);
			exec(2, 0, 2);
			exec(5, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[5][1][N - 1] == colorDown && faces[4][1][0] == colorSide) {
			exec(4, 0, 1);
			exec(2, 0, -1);
			exec(4, 0, -1);
			exec(0, 0, 2);
		}
		// edge between left + front
		else if (faces[0][1][0] == colorDown && faces[4][1][N - 1] == colorSide) {
			exec(4, 0, -1);
			exec(2, 0, -1);
			exec(4, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[4][1][N - 1] == colorDown && faces[0][1][0] == colorSide) {
			exec(0, 0, -1);
		}
		// edge between front + up
		else if (faces[2][N - 1][1] == colorDown && faces[0][0][1] == colorSide) {
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[0][0][1] == colorDown && faces[2][N - 1][1] == colorSide) {
			exec(2, 0, -1);
			exec(1, 0, -1);
			exec(0, 0, 1);
			exec(1, 0, 1);
		}
		// edge between right + up
		else if (faces[2][N - 2][N - 1] == colorDown && faces[1][0][1] == colorSide) {
			exec(2, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[1][0][1] == colorDown && faces[2][N - 2][N - 1] == colorSide) {
			exec(1, 0, -1);
			exec(0, 0, 1);
			exec(1, 0, 1);
		}
		// edge between back + up
		else if (faces[2][0][1] == colorDown && faces[5][0][1] == colorSide) {
			exec(2, 0, 2);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[5][0][1] == colorDown && faces[2][0][1] == colorSide) {
			exec(2, 0, 1);
			exec(1, 0, -1);
			exec(0, 0, 1);
			exec(1, 0, 1);
		}
		// edge between left + up
		else if (faces[2][1][0] == colorDown && faces[4][0][1] == colorSide) {
			exec(2, 0, -1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (faces[4][0][1] == colorDown && faces[2][1][0] == colorSide) {
			exec(4, 0, 1);
			exec(0, 0, -1);
			exec(4, 0, 1);
		}

		// once done, move finished edge to another edge on bottom
		exec(3, 0, -1);
	}

}

void Solver::solveCorners4() {
	
	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';

	int colorDown = getFaceColor(3);
	std::vector<int> sideColors{ getFaceColor(0), getFaceColor(1), getFaceColor(5), getFaceColor(4) };

	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		std::vector<int> desired{ colorDown, sideColors[numTimes], sideColors[(numTimes + 1) % 4] };
		std::sort(desired.begin(), desired.end());

		//std::cout << "numTimes = " << numTimes << std::endl;
		//std::cout << "Desired = " << myMap[desired[0]] << " " << myMap[desired[1]] << " " << myMap[desired[2]] << std::endl;

		// corner meeting front, right, down
		std::vector<int> corner{ faces[0][N - 1][N - 1], faces[1][N - 1][0], faces[3][0][N - 1] };
		std::sort(corner.begin(), corner.end());
		if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {
			
			//std::cout << "front, right, down" << std::endl;
			
			// if cubie in right place
			if (faces[3][0][N - 1] == colorDown) {
				// already in place
			}
			// if down color on front face
			else if (faces[0][N - 1][N - 1] == colorDown) {
				exec(0, 0, -1);
				exec(2, 0, -1);
				exec(0, 0, 1);
				exec(2, 0, 2);
				exec(1, 0, 1);
				exec(2, 0, -1);
				exec(1, 0, -1);
			}
			// if down color on right face
			else if (faces[1][N - 1][0] == colorDown) {
				exec(1, 0, 1);
				exec(2, 0, 1);
				exec(1, 0, -1);
				exec(2, 0, -2);
				exec(0, 0, -1);
				exec(2, 0, 1);
				exec(0, 0, 1);
			}

			//continue;
		}
		else {
			// corner meeting front, left, down
			corner[0] = faces[0][N - 1][0];
			corner[1] = faces[4][N - 1][N - 1];
			corner[2] = faces[3][0][0];
			std::sort(corner.begin(), corner.end());

			if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {

				//std::cout << "front, left, down" << std::endl;

				// if down color on down face
				if (faces[3][0][0] == colorDown) {
					exec(4, 0, -1);
					exec(2, 0, -2);
					exec(4, 0, 1);
					exec(0, 0, -1);
					exec(2, 0, 1);
					exec(0, 0, 1);
				}
				// if down color on front face
				else if (faces[0][N - 1][0] == colorDown) {
					exec(0, 0, 1);
					exec(2, 0, -2);
					exec(0, 0, -2);
					exec(2, 0, 1);
					exec(0, 0, 1);
				}
				// if down color on left face
				else if (faces[4][N - 1][N - 1] == colorDown) {
					exec(4, 0, -1);
					exec(2, 0, -1);
					exec(4, 0, 1);
					exec(2, 0, 1);
					exec(1, 0, 1);
					exec(2, 0, -1);
					exec(1, 0, -1);
				}

				//continue;
			}
			else {
				// corner meeting left, down, back
				corner[0] = faces[4][N - 1][0];
				corner[1] = faces[3][N - 1][0];
				corner[2] = faces[5][N - 1][N - 1];
				std::sort(corner.begin(), corner.end());

				if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {

					//std::cout << "left, down, back" << std::endl;

					// if down color on down face
					if (faces[3][N - 1][0] == colorDown) {
						exec(5, 0, -1);
						exec(2, 0, -1);
						exec(5, 0, 1);
						exec(2, 0, 2);
						exec(0, 0, -1);
						exec(2, 0, 1);
						exec(0, 0, 1);
					}
					// if down color on left face
					else if (faces[4][N - 1][0] == colorDown) {
						exec(4, 0, 1);
						exec(2, 0, 1);
						exec(4, 0, -1);
						exec(0, 0, -1);
						exec(2, 0, 1);
						exec(0, 0, 1);
					}
					// if down color on back face
					else if (faces[5][N - 1][N - 1] == colorDown) {
						exec(5, 0, -1);
						exec(2, 0, -1);
						exec(5, 0, 1);
						exec(1, 0, 1);
						exec(2, 0, -1);
						exec(1, 0, -1);
					}

					//continue;
				}
				else {
					// corner meeting right, back, down
					corner[0] = faces[1][N - 1][N - 1];
					corner[1] = faces[5][N - 1][0];
					corner[2] = faces[3][N - 1][N - 1];
					std::sort(corner.begin(), corner.end());

					if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {

						//std::cout << "right, back, down" << std::endl;

						// if down color on down face
						if (faces[3][N - 1][N - 1] == colorDown) {
							exec(5, 0, 1);
							exec(2, 0, 2);
							exec(5, 0, -1);
							exec(1, 0, 1);
							exec(2, 0, -1);
							exec(1, 0, -1);
						}
						// if down color on right face
						else if (faces[1][N - 1][N - 1] == colorDown) {
							exec(1, 0, -1);
							exec(2, 0, 2);
							exec(1, 0, 2);
							exec(2, 0, -1);
							exec(1, 0, -1);
						}
						// if down color on back face
						else if (faces[5][N - 1][0] == colorDown) {
							exec(5, 0, 1);
							exec(2, 0, 1);
							exec(5, 0, -1);
							exec(2, 0, -1);
							exec(0, 0, -1);
							exec(2, 0, 1);
							exec(0, 0, 1);
						}

						//continue;
					}
					else {
						// corner meeting front, right, up
						do {
							exec(2, 0, 1);
							corner[0] = faces[0][0][N - 1];
							corner[1] = faces[1][0][0];
							corner[2] = faces[2][N - 1][N - 1];
							std::sort(corner.begin(), corner.end());
						} while (desired[0] != corner[0] || desired[1] != corner[1] || desired[2] != corner[2]);

						//std::cout << "yay front, right, up" << std::endl;

						// if down color on up face
						if (faces[2][N - 1][N - 1] == colorDown) {
							exec(1, 0, 1);
							exec(2, 0, -1);
							exec(1, 0, -1);
							exec(0, 0, -1);
							exec(2, 0, 2);
							exec(0, 0, 1);
						}
						// if down color on right face
						else if (faces[1][0][0] == colorDown) {
							exec(2, 0, -1);
							exec(0, 0, -1);
							exec(2, 0, 1);
							exec(0, 0, 1);
						}
						// if down color on front face
						else if (faces[0][0][N - 1] == colorDown) {
							exec(2, 0, 1);
							exec(1, 0, 1);
							exec(2, 0, -1);
							exec(1, 0, -1);
						}


					}


				}
			}
		}


		// finish everything off by moving the finished cube off to the side
		exec(3, 0, -1);
		
	}

}

void Solver::solveSecondLayer() {
	if (N <= 2) {
		return;
	}

	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';

	// relative positioning:
	// first 6 are standard: front, right, up, down, left, back
	// next 6 are if we take absolute Right as relative front, with Up the same face:
	//      then relative front,right,up,down,left,back correspond to absolute faces 1,5,2,3,0,4
	// next 6 are if we take absolute Back as relative front
	// last 6 are if we take absolute Left as relative front
	std::vector<int> orient{ 0, 1, 2, 3, 4, 5, 1, 5, 2, 3, 0, 4, 5, 4, 2, 3, 1, 0, 4, 0, 2, 3, 5, 1 };

	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		int front = orient[0 + numTimes * 6];
		int right = orient[1 + numTimes * 6];
		int up = orient[2 + numTimes * 6];
		int left = orient[4 + numTimes * 6];
		int back = orient[5 + numTimes * 6];

		int colorFront = getFaceColor(front);
		int colorRight = getFaceColor(right);
		//std::cout << "looking for " << myMap[colorFront] << " " << myMap[colorRight] << std::endl;

		// 8 if statements: 4 for position in the middle (not on down or up face), and * 2 for if flipped
		// if edge already in correct place
		if (faces[front][1][N - 1] == colorFront && faces[right][1][0] == colorRight) {
			continue;
		}
		// edge flipped around
		else if(faces[right][1][0] == colorFront && faces[front][1][N - 1] == colorRight) {
			//std::cout << "orig part 2" << std::endl;
			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);
			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);

			exec(up, 0, 1);
			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);
			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);
		}
		// desired edge between left + front
		else if (faces[left][1][N - 1] == colorFront && faces[front][1][0] == colorRight) {
			//std::cout << "left + front" << std::endl;
			exec(left, 0, -1);
			exec(up, 0, 1);
			exec(left, 0, 1);
			exec(up, 0, 1);
			exec(front, 0, 1);
			exec(up, 0, -1);
			exec(front, 0, -1);

			exec(up, 0, -1);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);
			exec(up, 0, -1);
			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);
		}
		// flipped other way
		else if (faces[front][1][0] == colorFront && faces[left][1][N - 1] == colorRight) {
			//std::cout << "left + front part 2" << std::endl;
			exec(left, 0, -1);
			exec(up, 0, 1);
			exec(left, 0, 1);
			exec(up, 0, 1);
			exec(front, 0, 1);
			exec(up, 0, -1);
			exec(front, 0, -1);

			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);
			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);
		}
		// desired edge between left + back
		else if (faces[back][1][N - 1] == colorFront && faces[left][1][0] == colorRight) {
			//std::cout << "left + back" << std::endl;
			exec(back, 0, -1);
			exec(up, 0, 1);
			exec(back, 0, 1);
			exec(up, 0, 1);
			exec(left, 0, 1);
			exec(up, 0, -1);
			exec(left, 0, -1);

			exec(up, 0, 2);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);
			exec(up, 0, -1);
			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);
		}
		// flipped other way
		else if (faces[left][1][0] == colorFront && faces[back][1][N - 1] == colorRight) {
			//std::cout << "left + back part 2" << std::endl;
			exec(back, 0, -1);
			exec(up, 0, 1);
			exec(back, 0, 1);
			exec(up, 0, 1);
			exec(left, 0, 1);
			exec(up, 0, -1);
			exec(left, 0, -1);

			exec(up, 0, -1);
			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);
			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);
		}
		// desired edge between right + back
		else if (faces[back][1][0] == colorFront && faces[right][1][N - 1] == colorRight) {
			//std::cout << "right + back" << std::endl;
			exec(right, 0, -1);
			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, 1);
			exec(back, 0, 1);
			exec(up, 0, -1);
			exec(back, 0, -1);

			exec(up, 0, 2);
			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);
			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);

		}
		// flipped other way
		else if (faces[right][1][N - 1] == colorFront && faces[back][1][0] == colorRight) {
			//std::cout << "right + back part 2" << std::endl;
			exec(right, 0, -1);
			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, 1);
			exec(back, 0, 1);
			exec(up, 0, -1);
			exec(back, 0, -1);

			exec(up, 0, 1);
			exec(right, 0, 1);
			exec(up, 0, -1);
			exec(right, 0, -1);
			exec(up, 0, -1);
			exec(front, 0, -1);
			exec(up, 0, 1);
			exec(front, 0, 1);

		}
		else {
			std::cout << "top" << std::endl;
			// need to access correct sticker on Up face, depending on which edge it is solving right now
			// need to take correct sticker on Up, starting with if our relative Front is absolute Front, absolute Right, abs Back, or abs Left
			std::vector<int> topRotationRow{ N - 1, 1, 0, 1 };
			std::vector<int> topRotationCol{ 1, N - 1, 1, 0 };

			int topRow = topRotationRow[numTimes];
			int topCol = topRotationCol[numTimes];

			std::vector<int> desired{ colorFront, colorRight };
			std::sort(desired.begin(), desired.end());

			//std::cout << "desired = " << desired[0] << " " << desired[1] << std::endl;

			std::vector<int> myEdge;

			// turn the Up face until the edge you need is in relative position front + up
			do {
				exec(up, 0, 1);
				myEdge.clear();
				myEdge.push_back(faces[front][0][1]);
				myEdge.push_back(faces[up][topRow][topCol]);
				std::sort(myEdge.begin(), myEdge.end());
			} while (desired[0] != myEdge[0] || desired[1] != myEdge[1]);

			// now that our desired piece is in the edge between Up face and relative Front face, handle appropriately, depending on orientation
			if (faces[front][0][1] == colorFront) {
				exec(up, 0, 1);
				exec(right, 0, 1);
				exec(up, 0, -1);
				exec(right, 0, -1);
				exec(up, 0, -1);
				exec(front, 0, -1);
				exec(up, 0, 1);
				exec(front, 0, 1);
			}
			else {
				exec(up, 0, 2);
				exec(front, 0, -1);
				exec(up, 0, 1);
				exec(front, 0, 1);
				exec(up, 0, 1);
				exec(right, 0, 1);
				exec(up, 0, -1);
				exec(right, 0, -1);
			}
		}

		// by the time we get here, we have placed the desired edge into the second layer
	}

}

// https://ruwix.com/twisty-puzzles/4x4x4-rubiks-cube-rubiks-revenge/
// flips edge between Front + Up faces of even size cubes
void Solver::fixParityEdge() {

	int half = N / 2;

	for (int i = 1; i < half; ++i) {
		exec(1, i, 2);
	}
	exec(5, 0, 2);
	exec(2, 0, 2);
	for (int i = 1; i < half; ++i) {
		exec(4, i, 1);
	}

	exec(2, 0, 2);
	for (int i = 1; i < half; ++i) {
		exec(1, i, -1);
	}
	exec(2, 0, 2);
	for (int i = 1; i < half; ++i) {
		exec(1, i, 1);
	}

	exec(2, 0, 2);
	exec(0, 0, 2);
	for (int i = 1; i < half; ++i) {
		exec(1, i, 1);
	}
	exec(0, 0, 2);

	for (int i = 1; i < half; ++i) {
		exec(4, i, -1);
	}
	exec(5, 0, 2);
	for (int i = 1; i < half; ++i) {
		exec(1, i, 2);
	}


	/*
	for (int i = 0; i < half; ++i) {
		exec(1, i, 1);
	}
	exec(5, 0, -1);

	for (int i = 0; i < N; i++) {
		exec(0, 0, -1);
	}

	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < half; ++i) {
			exec(1, i, -1);
		}
		exec(0, 0, 1);
		exec(2, 0, 2);
		exec(0, 0, -1);
	}

	for (int i = 1; i < half; ++i) {
		exec(1, i, -1);
	}
	
	for (int i = 0; i < N; i++) {
		exec(0, 0, 1);
	}

	exec(5, 0, 1);

	for (int i = 0; i < half; ++i) {
		exec(1, 0, -1);
	}

	*/

	/*
	for (int i = 0; i < half; ++i) {
		exec(1, i, -1);
	}

	for (int numTimes = 0; numTimes < 5; ++numTimes) {
		exec(0, 0, 2);
		exec(2, 0, -1);

		for (int i = 0; i < half; i++) {
			exec(4, i, -1);
		}

		exec(2, 0, 1);
	}

	for (int i = 0; i < half; ++i) {
		exec(1, i, 1);
	}
	*/
	
}

// https://ruwix.com/twisty-puzzles/4x4x4-rubiks-cube-rubiks-revenge/
// flips corner pieces Front-Right-Up and Back-Left-Up
void Solver::fixParityOppositeCorners() {
	int half = N / 2;
	for (int i = 0; i < half; ++i) {
		exec(1, i, 2);
	}
	for (int i = 1; i < half; ++i) {
		exec(0, i, 2); 
	}
	exec(2, 0, 2);
	for (int i = 0; i < half; ++i) {
		exec(0, i, 2);
	}

	exec(3, 0, 1);
	for (int i = 0; i < half; ++i) {
		exec(1, i, 2);
	}
	exec(2, 0, 2);
	for (int i = 0; i < half; ++i) {
		exec(0, i, 2);
	}
	
	exec(2, 0, -1);
	for (int i = 0; i < half; ++i) {
		exec(0, i, 2);
	}
	exec(4, 0, 2);
	exec(2, 0, 2);

	exec(5, 0, 2);
	for (int i = 0; i < half; ++i) {
		exec(4, i, 2);
	}
	exec(2, 0, 1);


}

// https://ruwix.com/twisty-puzzles/4x4x4-rubiks-cube-rubiks-revenge/
// flips corner pieces Front-Right-Up and Front-Left-Up
void Solver::fixParityAdjacentCorners() {
	int half = N / 2;

	exec(0, 0, 2);
	exec(1, 0, 2);
	exec(5, 0, -1);

	exec(3, 0, -1);
	exec(5, 0, 1);
	exec(1, 0, 2);
	exec(0, 0, -1);
	exec(2, 0, 1);
	
	for (int i = 0; i < half; ++i) {
		exec(0, i, 2);
	}
	exec(0, 0, 1);
	exec(4, 0, 2);

	for (int i = 1; i < half; ++i) {
		exec(0, i, 2);
	}
	for (int i = 0; i < half; ++i) {
		exec(4, i, 2);
	}

	for (int i = 1; i < half; ++i) {
		exec(0, i, 2);
	}
	for (int i = 1; i < half; ++i) {
		exec(4, i, 2);
	}
	exec(2, 0, -1);
}

void Solver::solveLastCross() {
	// small enough cubes do not have edges
	if (N <= 2) {
		return;
	}

	int colorTop = getFaceColor(2);

	bool front = faces[2][N - 1][1] == colorTop;
	bool right = faces[2][1][N - 1] == colorTop;
	bool back = faces[2][0][1] == colorTop;
	bool left = faces[2][1][0] == colorTop;

	int count = front + right + back + left;

	if (N % 2 == 0 && count % 2 == 1) {
		std::cout << front << " " << right << " " << back << " " << left << std::endl;
		std::cout << "BADDDDDDDDDd" << std::endl;
		//fixParityEdge();

		for (int i = 0; i < 4; ++i) {
			if (faces[2][N - 1][1] == colorTop) {
				fixParityEdge();
			}
			exec(2, 0, 1);
		}

		front = faces[2][N - 1][1] == colorTop;
		right = faces[2][1][N - 1] == colorTop;
		back = faces[2][0][1] == colorTop;
		left = faces[2][1][0] == colorTop;

		count = front + right + back + left;

		std::cout << front << " " << right << " " << back << " " << left << std::endl;

		//return;
	}

	int rawr = 0;

	while (count < 4) {
		if (count >= 2) {
			while (!(left && back) && !(left && right)) {
				exec(2, 0, 1);

				front = faces[2][N - 1][1] == colorTop;
				right = faces[2][1][N - 1] == colorTop;
				back = faces[2][0][1] == colorTop;
				left = faces[2][1][0] == colorTop;
			}
		}

		exec(0, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(1, 0, -1);
		exec(0, 0, -1);

		front = faces[2][N - 1][1] == colorTop;
		right = faces[2][1][N - 1] == colorTop;
		back = faces[2][0][1] == colorTop;
		left = faces[2][1][0] == colorTop;

		count = front + right + back + left;
		std::cout << "COUNT = " << count << std::endl;

		rawr++;

		if (rawr == 20) {
			break;
		}
	}

}

void Solver::solveLastEdges() {
	// small enough cubes do not have edges
	if (N <= 2) {
		return;
	}

	int colorUp = getFaceColor(2);
	int colorBack = getFaceColor(5);
	int colorFront = getFaceColor(0);
	int colorLeft = getFaceColor(4);
	int colorRight = getFaceColor(1);

	// orient last layer so that back face edge has correct color
	while (faces[5][0][1] != colorBack) {
		exec(2, 0, 1);
	}

	// 6 possible ways that the other 3 edges are positioned; one of them is solved state
	// from front view, we want Orange, Green, Red on Left, Front, Right
	// could have (Green, Orange, Red)
	if (faces[4][0][1] == colorFront && faces[0][0][1] == colorLeft) {
		// the following swaps up-front and up-left
		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);
	}
	// could have (Red, Green, Orange)
	else if (faces[4][0][1] == colorRight && faces[1][0][1] == colorLeft) {
		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);

		// now Green, Red, Orange
		exec(2, 0, 1);

		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);

		exec(2, 0, -1);
		// now Green, Orange, Red

		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);
		// now Orange, Green, Red
	}
	// could have Orange, Red, Green
	else if (faces[0][0][1] == colorRight && faces[1][0][1] == colorFront) {
		exec(2, 0, 1);

		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);

		exec(2, 0, -1);
	}
	// could have Green, Red, Orange
	else if (faces[4][0][1] == colorFront && faces[0][0][1] == colorRight && faces[1][0][1] == colorLeft) {
		exec(2, 0, 1);

		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);

		exec(2, 0, -1);
		// now Green, Orange, Red

		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);
		// now Orange, Green, Red
	}
	// coudl have Red, Orange, Green
	else if (faces[4][0][1] == colorRight && faces[0][0][1] == colorLeft && faces[1][0][1] == colorFront) {
		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);
		// now Orange, Red, Green

		exec(2, 0, 1);

		exec(1, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, 2);
		exec(1, 0, -1);
		exec(2, 0, 1);

		exec(2, 0, -1);
		// now Orange, Green, Red
	}
}

void Solver::solveLastCornerPosition() {
	int colorFront = getFaceColor(0);
	int colorRight = getFaceColor(1);
	int colorUp = getFaceColor(2);
	int colorLeft = getFaceColor(4);
	int colorBack = getFaceColor(5);

	// we need corner in front-right-up to be correct
	std::vector<int> desired1{ colorUp, colorFront, colorRight };
	std::sort(desired1.begin(), desired1.end());

	// corner between front-left-up
	std::vector<int> desired2{ colorUp, colorFront, colorLeft };
	std::sort(desired2.begin(), desired2.end());

	// corner between back-left-up
	std::vector<int> desired3{ colorUp, colorBack, colorLeft };
	std::sort(desired3.begin(), desired3.end());

	// corner between back-right-up
	std::vector<int> desired4{ colorUp, colorBack, colorRight };
	std::sort(desired4.begin(), desired4.end());

	std::vector<int> corner1{ faces[2][N - 1][N - 1], faces[0][0][N - 1], faces[1][0][0] };
	std::sort(corner1.begin(), corner1.end());

	std::vector<int> corner2{ faces[2][N - 1][0], faces[0][0][0], faces[4][0][N - 1] };
	std::sort(corner2.begin(), corner2.end());

	std::vector<int> corner3{ faces[2][0][0], faces[5][0][N - 1], faces[4][0][0] };
	std::sort(corner3.begin(), corner3.end());

	std::vector<int> corner4{ faces[2][0][N-1], faces[5][0][0], faces[1][0][N - 1] };
	std::sort(corner4.begin(), corner4.end());

	bool same1 = (desired1[0] == corner1[0] && desired1[1] == corner1[1] && desired1[2] == corner1[2]);
	bool same2 = (desired2[0] == corner2[0] && desired2[1] == corner2[1] && desired2[2] == corner2[2]);
	bool same3 = (desired3[0] == corner3[0] && desired3[1] == corner3[1] && desired3[2] == corner3[2]);
	bool same4 = (desired4[0] == corner4[0] && desired4[1] == corner4[1] && desired4[2] == corner4[2]);
	
	if (same1 + same2 + same3 + same4 == 2) {
		std::cout << "BAD CORNERS!!!" << std::endl;
		if (same1 && same2) {
			fixParityAdjacentCorners();
		}
		else if (same1 && same3) {
			fixParityOppositeCorners();
		}
		else if (same1 && same4) {
			exec(2, 0, 1);
			fixParityAdjacentCorners();
			exec(2, 0, -1);
		}
		else if (same2 && same3) {
			exec(2, 0, -1);
			fixParityAdjacentCorners();
			exec(2, 0, 1);
		}
		else if (same2 && same4) {
			exec(2, 0, 1);
			fixParityOppositeCorners();
			exec(2, 0, -1);
		}
		else if (same3 && same4) {
			exec(2, 0, 2);
			fixParityAdjacentCorners();
			exec(2, 0, -2);
		}

		corner1[0] = faces[2][N - 1][N - 1];
		corner1[1] = faces[0][0][N - 1];
		corner1[2] = faces[1][0][0];
		std::sort(corner1.begin(), corner1.end());

		corner2[0] = faces[2][N - 1][0];
		corner2[1] = faces[0][0][0];
		corner2[2] = faces[4][0][N - 1];
		std::sort(corner2.begin(), corner2.end());

		corner3[0] = faces[2][0][0];
		corner3[1] = faces[5][0][N - 1];
		corner3[2] = faces[4][0][0];
		std::sort(corner3.begin(), corner3.end());

		corner4[0] = faces[2][0][N - 1];
		corner4[1] = faces[5][0][0];
		corner4[2] = faces[1][0][N - 1];
		std::sort(corner4.begin(), corner4.end());

		same1 = (desired1[0] == corner1[0] && desired1[1] == corner1[1] && desired1[2] == corner1[2]);
		same2 = (desired2[0] == corner2[0] && desired2[1] == corner2[1] && desired2[2] == corner2[2]);
		same3 = (desired3[0] == corner3[0] && desired3[1] == corner3[1] && desired3[2] == corner3[2]);
		same4 = (desired4[0] == corner4[0] && desired4[1] == corner4[1] && desired4[2] == corner4[2]);

	}

	// if none of them are right
	if (!same1 && !same2 && !same3 && !same4) {
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);

		corner1[0] = faces[2][N - 1][N - 1];
		corner1[1] = faces[0][0][N - 1];
		corner1[2] = faces[1][0][0];
		std::sort(corner1.begin(), corner1.end());

		corner2[0] = faces[2][N - 1][0];
		corner2[1] = faces[0][0][0];
		corner2[2] = faces[4][0][N - 1];
		std::sort(corner2.begin(), corner2.end());

		corner3[0] = faces[2][0][0];
		corner3[1] = faces[5][0][N - 1];
		corner3[2] = faces[4][0][0];
		std::sort(corner3.begin(), corner3.end());

		corner4[0] = faces[2][0][N - 1];
		corner4[1] = faces[5][0][0];
		corner4[2] = faces[1][0][N - 1];
		std::sort(corner4.begin(), corner4.end());

		same1 = (desired1[0] == corner1[0] && desired1[1] == corner1[1] && desired1[2] == corner1[2]);
		same2 = (desired2[0] == corner2[0] && desired2[1] == corner2[1] && desired2[2] == corner2[2]);
		same3 = (desired3[0] == corner3[0] && desired3[1] == corner3[1] && desired3[2] == corner3[2]);
		same4 = (desired4[0] == corner4[0] && desired4[1] == corner4[1] && desired4[2] == corner4[2]);
	}

	int relFront = 0;
	int relRight = 1;
	int relUp = 2;
	int relLeft = 4;
	int index = 0;
	if (same1) {
		index = 0;
	}
	else if (same2) {
		index = 2;
		relFront = 4;
		relRight = 0;
		relLeft = 5;
	}
	else if (same3) {
		index = 3;
		relFront = 5;
		relRight = 4;
		relLeft = 1;
	}
	else if (same4) {
		index = 4;
		relFront = 1;
		relRight = 5;
		relLeft = 0;
	}

	std::cout << "relFront = " << relFront << std::endl;

	int numTimes = 0;
	while ((!same1 || !same2 || !same3 || !same4) && numTimes < 2) {
		numTimes++;

		exec(relUp, 0, 1);
		exec(relRight, 0, 1);
		exec(relUp, 0, -1);
		exec(relLeft, 0, -1);
		exec(relUp, 0, 1);
		exec(relRight, 0, -1);
		exec(relUp, 0, -1);
		exec(relLeft, 0, 1);

		corner1[0] = faces[2][N - 1][N - 1];
		corner1[1] = faces[0][0][N - 1];
		corner1[2] = faces[1][0][0];
		std::sort(corner1.begin(), corner1.end());

		corner2[0] = faces[2][N - 1][0];
		corner2[1] = faces[0][0][0];
		corner2[2] = faces[4][0][N - 1];
		std::sort(corner2.begin(), corner2.end());

		corner3[0] = faces[2][0][0];
		corner3[1] = faces[5][0][N - 1];
		corner3[2] = faces[4][0][0];
		std::sort(corner3.begin(), corner3.end());

		corner4[0] = faces[2][0][N - 1];
		corner4[1] = faces[5][0][0];
		corner4[2] = faces[1][0][N - 1];
		std::sort(corner4.begin(), corner4.end());

		same1 = (desired1[0] == corner1[0] && desired1[1] == corner1[1] && desired1[2] == corner1[2]);
		same2 = (desired2[0] == corner2[0] && desired2[1] == corner2[1] && desired2[2] == corner2[2]);
		same3 = (desired3[0] == corner3[0] && desired3[1] == corner3[1] && desired3[2] == corner3[2]);
		same4 = (desired4[0] == corner4[0] && desired4[1] == corner4[1] && desired4[2] == corner4[2]);
	}
}

// https://ruwix.com/the-rubiks-cube/how-to-solve-the-rubiks-cube-beginners-method/ for reference
void Solver::solve3x3x3() {
	solveCross();
	solveCorners4();
	solveSecondLayer();
	solveLastCross();
	solveLastEdges();
	solveLastCornerPosition();
}