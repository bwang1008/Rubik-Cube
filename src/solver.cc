#include "solver.h"
#include <map>


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

	dequePtr->push_back(glm::vec3(face, layer, qt));
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

void Solver::incr() {
	state++;
}

int Solver::currentState() {
	return state;
}

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
			//completed = true;
			//std::cout << "completed = " << completed << std::endl;
			//std::cout << "true, false = " << true << " " << false << std::endl;


		} // end while; band parallel to down face

		// orient band back to original
		exec(0, 0, 1);

		// put band/layer back in bottom face
		exec(4, i, 1);
	}
}