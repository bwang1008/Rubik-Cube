#include "solver.h"
#include <map>		// std::map for debugging purposes

Solver::Solver() {
	N = kCubeWidth;
	// state = 0;
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < kCubeWidth; ++j) {
			for (int k = 0; k < kCubeWidth; ++k) {
				faces[i][j][k] = i;
			}
		}
	}
	dequePtr = nullptr;
	storeTopColor = getFaceColor(2);

	for(int i = 0; i < 6; ++i) {
		facePos[i] = 0;
	}
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
int Solver::get(int face, int row, int col) {
	// row, col 0-indexed from top right
	// return faces[face][row][col];

	int rotationPos = facePos[face];
	if(rotationPos == 1) {
		return faces[face][kCubeWidth - col - 1][row];
	}
	else if(rotationPos == 2) {
		return faces[face][kCubeWidth - row - 1][kCubeWidth - col - 1];
	}
	else if(rotationPos == 3) {
		return faces[face][col][kCubeWidth - row - 1];
	}
	
	return faces[face][row][col];
}

// Sets the color of the sticker, given face, row, column, into the specified color (0-5)
// should only be called in turnFront, turnRight,...
void Solver::set(int face, int row, int col, int color) {
	int rotationPos = facePos[face];
	if(rotationPos == 1) {
		faces[face][kCubeWidth - col - 1][row] = color;
	}
	else if(rotationPos == 2) {
		faces[face][kCubeWidth - row - 1][kCubeWidth - col - 1] = color;
	}
	else if(rotationPos == 3) {
		faces[face][col][kCubeWidth - row - 1] = color;
	}

	faces[face][row][col] = color;
}

// exec calls this when face = 0
void Solver::turnFront(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		facePos[0] = (facePos[0] + qts) % 4;
	}
	else if (layer == N - 1) {
		facePos[5] = (facePos[5] + 4 - qts) % 4;
	}

	// now move the bands 
	int upBand[kCubeWidth];
	int rightBand[kCubeWidth];
	int downBand[kCubeWidth];
	int leftBand[kCubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = get(2, N - 1 - layer, i);
		rightBand[i] = get(1, i, layer);
		downBand[i] = get(3, layer, N - 1 - i);
		leftBand[i] = get(4, N - 1 - i, N - 1 - layer);
	}
	
	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			set(2, N - 1 - layer, i, leftBand[i]);
			set(1, i, layer, upBand[i]);
			set(3, layer, N - 1 - i, rightBand[i]);
			set(4, N - 1 - i, N - 1 - layer, downBand[i]);
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			set(2, N - 1 - layer, i, downBand[i]);
			set(1, i, layer, leftBand[i]);
			set(3, layer, N - 1 - i, upBand[i]);
			set(4, N - 1 - i, N - 1 - layer, rightBand[i]);
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			set(2, N - 1 - layer, i, rightBand[i]);
			set(1, i, layer, downBand[i]);
			set(3, layer, N - 1 - i, leftBand[i]);
			set(4, N - 1 - i, N - 1 - layer, upBand[i]);
		}
	}
}

// exec calls this when face = 1
void Solver::turnRight(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		facePos[1] = (facePos[1] + qts) % 4;
	}
	else if (layer == N - 1) {
		facePos[4] = (facePos[4] + 4 - qts) % 4;
	}

	// now move the bands 
	int upBand[kCubeWidth];
	int backBand[kCubeWidth];
	int downBand[kCubeWidth];
	int frontBand[kCubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = get(2, N - 1 - i, N - 1 - layer);
		backBand[i] = get(5, i, layer);
		downBand[i] = get(3, N - 1 - i, N - 1 - layer);
		frontBand[i] = get(0, N - 1 - i, N - 1 - layer);
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			set(2, N - 1 - i, N - 1 - layer, frontBand[i]);
			set(5, i, layer, upBand[i]);
			set(3, N - 1 - i, N - 1 - layer, backBand[i]);
			set(0, N - 1 - i, N - 1 - layer, downBand[i]);
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			set(2, N - 1 - i, N - 1 - layer, downBand[i]);
			set(5, i, layer, frontBand[i]);
			set(3, N - 1 - i, N - 1 - layer, upBand[i]);
			set(0, N - 1 - i, N - 1 - layer, backBand[i]);
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			set(2, N - 1 - i, N - 1 - layer, backBand[i]);
			set(5, i, layer, downBand[i]);
			set(3, N - 1 - i, N - 1 - layer, frontBand[i]);
			set(0, N - 1 - i, N - 1 - layer, upBand[i]);
		}
	}
}

// exec calls this when face = 2
void Solver::turnUp(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		facePos[2] = (facePos[2] + qts) % 4;
	}
	else if (layer == N - 1) {
		facePos[3] = (facePos[3] + 4 - qts) % 4;
	}

	// now move the bands 
	int backBand[kCubeWidth];
	int rightBand[kCubeWidth];
	int frontBand[kCubeWidth];
	int leftBand[kCubeWidth];

	for (int i = 0; i < N; ++i) {
		backBand[i] = get(5, layer, N - 1 - i);
		rightBand[i] = get(1, layer, N - 1 - i);
		frontBand[i] = get(0, layer, N - 1 - i);
		leftBand[i] = get(4, layer, N - 1 - i);
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			set(5, layer, N - 1 - i, leftBand[i]);
			set(1, layer, N - 1 - i, backBand[i]);
			set(0, layer, N - 1 - i, rightBand[i]);
			set(4, layer, N - 1 - i, frontBand[i]);
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			set(5, layer, N - 1 - i, frontBand[i]);
			set(1, layer, N - 1 - i, leftBand[i]);
			set(0, layer, N - 1 - i, backBand[i]);
			set(4, layer, N - 1 - i, rightBand[i]);
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			set(5, layer, N - 1 - i, rightBand[i]);
			set(1, layer, N - 1 - i, frontBand[i]);
			set(0, layer, N - 1 - i, leftBand[i]);
			set(4, layer, N - 1 - i, backBand[i]);
		}
	}
}

// exec calls this when face = 3
void Solver::turnDown(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		facePos[3] = (facePos[3] + qts) % 4;
	}
	else if (layer == N - 1) {
		facePos[2] = (facePos[2] + 4 - qts) % 4;
	}

	// now move the bands 
	int frontBand[kCubeWidth];
	int rightBand[kCubeWidth];
	int backBand[kCubeWidth];
	int leftBand[kCubeWidth];

	for (int i = 0; i < N; ++i) {
		frontBand[i] = get(0, N - 1 - layer, i);
		rightBand[i] = get(1, N - 1 - layer, i);
		backBand[i] = get(5, N - 1 - layer, i);
		leftBand[i] = get(4, N - 1 - layer, i);
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			set(0, N - 1 - layer, i, leftBand[i]);
			set(1, N - 1 - layer, i, frontBand[i]);
			set(5, N - 1 - layer, i, rightBand[i]);
			set(4, N - 1 - layer, i, backBand[i]);
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			set(0, N - 1 - layer, i, backBand[i]);
			set(1, N - 1 - layer, i, leftBand[i]);
			set(5, N - 1 - layer, i, frontBand[i]);
			set(4, N - 1 - layer, i, rightBand[i]);
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			set(0, N - 1 - layer, i, rightBand[i]);
			set(1, N - 1 - layer, i, backBand[i]);
			set(5, N - 1 - layer, i, leftBand[i]);
			set(4, N - 1 - layer, i, frontBand[i]);
		}
	}
}

// exec calls this when face = 4
void Solver::turnLeft(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		facePos[4] = (facePos[4] + qts) % 4;
	}
	else if (layer == N - 1) {
		facePos[1] = (facePos[1] + 4 - qts) % 4;
	}

	// now move the bands 
	int upBand[kCubeWidth];
	int frontBand[kCubeWidth];
	int downBand[kCubeWidth];
	int backBand[kCubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = get(2, i, layer);
		frontBand[i] = get(0, i, layer);
		downBand[i] = get(3, i, layer);
		backBand[i] = get(5, N - 1 - i, N - 1 - layer);
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			set(2, i, layer, backBand[i]);
			set(0, i, layer, upBand[i]);
			set(3, i, layer, frontBand[i]);
			set(5, N - 1 - i, N - 1 - layer, downBand[i]);
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			set(2, i, layer, downBand[i]);
			set(0, i, layer, backBand[i]);
			set(3, i, layer, upBand[i]);
			set(5, N - 1 - i, N - 1 - layer, frontBand[i]);
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			set(2, i, layer, frontBand[i]);
			set(0, i, layer, downBand[i]);
			set(3, i, layer, backBand[i]);
			set(5, N - 1 - i, N - 1 - layer, upBand[i]);
		}
	}
}

// exec calls this when face = 5
void Solver::turnBack(int layer, int qts) {
	qts = ((qts % 4) + 4) % 4;
	if (qts == 0) {
		return;
	}

	if (layer == 0) {
		facePos[5] = (facePos[5] + qts) % 4;
	}
	else if (layer == N - 1) {
		facePos[0] = (facePos[0] + 4 - qts) % 4;
	}

	// now move the bands 
	int upBand[kCubeWidth];
	int leftBand[kCubeWidth];
	int downBand[kCubeWidth];
	int rightBand[kCubeWidth];

	for (int i = 0; i < N; ++i) {
		upBand[i] = get(2, layer, N - 1 - i);
		leftBand[i] = get(4, i, layer);
		downBand[i] = get(3, N - 1 - layer, i);
		rightBand[i] = get(1, N - 1 - i, N - 1 - layer);
	}

	if (qts == 1) {
		for (int i = 0; i < N; ++i) {
			set(2, layer, N - 1 - i, rightBand[i]);
			set(4, i, layer, upBand[i]);
			set(3, N - 1 - layer, i, leftBand[i]);
			set(1, N - 1 - i, N - 1 - layer, downBand[i]);
		}
	}
	else if (qts == 2) {
		for (int i = 0; i < N; ++i) {
			set(2, layer, N - 1 - i, downBand[i]);
			set(4, i, layer, rightBand[i]);
			set(3, N - 1 - layer, i, upBand[i]);
			set(1, N - 1 - i, N - 1 - layer, leftBand[i]);
		}
	}
	else {
		for (int i = 0; i < N; ++i) {
			set(2, layer, N - 1 - i, leftBand[i]);
			set(4, i, layer, downBand[i]);
			set(3, N - 1 - layer, i, rightBand[i]);
			set(1, N - 1 - i, N - 1 - layer, upBand[i]);
		}
	}
}

// wrapper function that turns internal representation, and writes to deque of moves
void Solver::exec(int face, int layer, int qt) {
	qt %= 4;
	switch (face) {
	case 0: turnFront(layer, qt); 	break;
	case 1: turnRight(layer, qt); 	break;
	case 2: turnUp(layer, qt); 	  	break;
	case 3: turnDown(layer, qt); 	break;
	case 4: turnLeft(layer, qt); 	break;
	case 5: turnBack(layer, qt); 	break;
	}

	if (dequePtr->size() > 0) {
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
			int key = get(0, i, j);
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	
	std::cout << "Right\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = get(1, i, j);
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Up\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = get(2, i, j);
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Down\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = get(3, i, j);
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Left\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = get(4, i, j);
			char c = myMap[key];
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Back\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			int key = get(5, i, j);
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
	// state++;
}

// get the current state
int Solver::currentState() {
	// return state;
	return 0;
}

// randomly adds moves to scramble a cube
void Solver::scrambleCube() {
	int N = kCubeWidth;
	srand(1);
	int numberMoves = std::min(3 * N * N, kMaxScramble);

	for (int i = 0; i < numberMoves; ++i) {
		int randFace = rand() % 3;
		int randLayer = rand() % N;
		int randRotation = rand() % 3;
		if (randRotation == 0)
			randRotation = -1; // now -1, 1, or 2

		exec(randFace, randLayer, randRotation);
	}
}

// solves most of Down face; uses less moves than calling solveCenter0 alone
// also puts most of stickers for back face onto the up face
void Solver::preliminary0() {
	if(kCubeWidth <= 3) {
		return;
	}
	
	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';

	int colorD = getFaceColor(3);

	// most of bottom face
	for (int i = 1; i < N - 1; ++i) {
		exec(4, i, -1);

		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, j, i) == colorD && get(0, j, i) != colorD) {
						exec(2, j, -1);
					}
				}
				exec(4, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, j, i) == colorD && get(0, j, i) != colorD) {
						exec(2, j, 1);
					}
				}
				exec(1, 0, 1);
			}
		}

		exec(4, i, 1);
	}

	// access to other two sides for bottom face
	for (int i = 1; i < N - 1; ++i) {
		exec(0, i, -1); // bring to Right

		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, j, i) == colorD && get(1, j, i) != colorD) {
						exec(2, j, -1);
					}
				}
				exec(0, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, j, i) == colorD && get(1, j, i) != colorD) {
						exec(2, j, 1);
					}
				}
				exec(5, 0, 1);
			}
		}

		exec(0, i, 1);
	}

	int colorB = getFaceColor(5);
	storeTopColor = colorB;

	// most of back face, but placing on Up face for now, except for center col of odd N
	for (int i = 1; i < N - 1; ++i) {
		if (2 * i + 1 == N) {
			continue;
		}

		exec(4, i, 1); // on Front face, parallel to Left
		exec(0, 0, 2); // on Front face, parallel to Right
		exec(4, i, -1); // restore bottom face
		
		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, j, N - 1 - i) == colorB && get(0, j, N - 1 - i) != colorB) {
						exec(2, j, -1);
					}
				}
				exec(4, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, j, N - 1 - i) == colorB && get(0, j, N - 1 - i) != colorB) {
						exec(2, j, 1);
					}
				}
				exec(1, 0, 1);
			}
		}

		exec(4, i, 1); // undo entrance
		exec(0, 0, 2);
		exec(4, i, -1);
	}

	// access to other two sides for back face that is on Up face
	for (int i = 1; i < N - 1; ++i) {
		exec(0, i, 1); // on Right face, parallel to Front
		exec(1, 0, 2); // on Right face, parallel to Back
		exec(0, i, -1); // restore bottom face

		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (2 * j + 1 == N) {
						continue;
					}

					if (get(0, j, N - 1 - i) == colorB && get(1, j, N - 1 - i) != colorB) {
						exec(2, j, -1);
					}
				}
				exec(0, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (2 * j + 1 == N) {
						continue;
					}

					if (get(5, j, N - 1 - i) == colorB && get(1, j, N - 1 - i) != colorB) {
						exec(2, j, 1);
					}
				}
				exec(5, 0, 1);
			}
		}

		exec(0, i, 1); // undo entrance
		exec(1, 0, 2);
		exec(0, i, -1);
	}

	// most of bottom face AGAIN
	for (int i = 1; i < N - 1; ++i) {
		exec(4, i, -1);
		exec(0, 0, 2);
		exec(4, i, 1);

		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, j, N - 1 - i) == colorD && get(0, j, N - 1 - i) != colorD) {
						exec(2, j, -1);
					}
				}
				exec(4, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, j, N - 1 - i) == colorD && get(0, j, N - 1 - i) != colorD) {
						exec(2, j, 1);
					}
				}
				exec(1, 0, 1);
			}
		}

		exec(4, i, -1);
		exec(0, 0, 2);
		exec(4, i, 1);
	}

	// access to other two sides for bottom face
	for (int i = 1; i < N - 1; ++i) {
		exec(0, i, -1); // bring to Right
		exec(1, 0, 2);
		exec(0, i, 1);

		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, j, N - 1 - i) == colorD && get(1, j, N - 1 - i) != colorD) {
						exec(2, j, -1);
					}
				}
				exec(0, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, j, N - 1 - i) == colorD && get(1, j, N - 1 - i) != colorD) {
						exec(2, j, 1);
					}
				}
				exec(5, 0, 1);
			}
		}

		exec(0, i, -1);
		exec(1, 0, 2);
		exec(0, i, 1);
	}
}

// solving Down face: use swapping method to get down stickers from Back, Left, Right, Front
// then be careful on getting up to preserve 2 quarter turns per mismatch of stickers
// Let M = number of mismatches on this face
// Even Worst-Case: 2M + 192(N-2) + 16 + 42(N-2)
// Odd  Worst-Case: 2M + 192(N-2) + 16 + 42(N-3) + 1 + 21(N-3)
void Solver::solveCenter0() {
	if(kCubeWidth <= 3) {
		return;
	}

	int colorD = getFaceColor(3);

	// get all colorD stickers from back
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(3, j, i) != colorD && get(5, N - 1 - j, N - 1 - i) == colorD) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(4, i, 1);
			exec(5, 0, -1);
			for (int r2 : indices) {
				exec(4, r2, 1);
			}
			exec(5, 0, 1);

			exec(4, i, -1);
			exec(5, 0, -1);
			for (int r2 : indices) {
				exec(4, r2, -1);
			}
			exec(5, 0, 1);

			if (doOther) {
				exec(4, i, 1);
				exec(5, 0, 1);
				exec(4, N - 1 - i, 1);
				exec(5, 0, -1);

				exec(4, i, -1);
				exec(5, 0, 1);
				exec(4, N - 1 - i, -1);
				exec(5, 0, -1);
			}
		}

		exec(5, 0, 1);
	}

	// get all colorD stickers from left
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(3, i, j) != colorD && get(4, j, N - 1 - i) == colorD) {
					if (N - 1 - i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(0, i, 1);
			exec(4, 0, -1);
			for (int r2 : indices) {
				exec(0, N - 1 - r2, 1);
			}
			exec(4, 0, 1);

			exec(0, i, -1);
			exec(4, 0, -1);
			for (int r2 : indices) {
				exec(0, N - 1 - r2, -1);
			}
			exec(4, 0, 1);

			if (doOther) {
				exec(0, i, 1);
				exec(4, 0, 1);
				exec(0, N - 1 - i, 1);
				exec(4, 0, -1);

				exec(0, i, -1);
				exec(4, 0, 1);
				exec(0, N - 1 - i, -1);
				exec(4, 0, -1);
			}
		}

		exec(4, 0, 1);
	}

	// get all colorD stickers from right
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(3, i, j) != colorD && get(1, N - 1 - j, i) == colorD) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(0, i, -1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(0, r2, -1);
			}
			exec(1, 0, -1);

			exec(0, i, 1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(0, r2, 1);
			}
			exec(1, 0, -1);

			if (doOther) {
				exec(0, i, -1);
				exec(1, 0, -1);
				exec(0, N - 1 - i, -1);
				exec(1, 0, 1);

				exec(0, i, 1);
				exec(1, 0, -1);
				exec(0, N - 1 - i, 1);
				exec(1, 0, 1);
			}
		}

		exec(1, 0, 1);
	}

	// get all colorD stickers from front
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(3, j, i) != colorD && get(0, j, i) == colorD) {
					if (N - 1 - i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(4, i, -1);
			exec(0, 0, 1);
			for (int r2 : indices) {
				exec(4, N - 1 - r2, -1);
			}
			exec(0, 0, -1);

			exec(4, i, 1);
			exec(0, 0, 1);
			for (int r2 : indices) {
				exec(4, N - 1 - r2, 1);
			}
			exec(0, 0, -1);

			if (doOther) {
				exec(4, i, -1);
				exec(0, 0, -1);
				exec(4, N - 1 - i, -1);
				exec(0, 0, 1);

				exec(4, i, 1);
				exec(0, 0, -1);
				exec(4, N - 1 - i, 1);
				exec(0, 0, 1);
			}
		}

		exec(0, 0, 1);
	}

	// get all colorD stickers fron up
	int howMany = (N % 2 == 1) ? 2 : 1;
	for (int numTimes = 0; numTimes < howMany; ++numTimes) {
		// bring each column from down into front, ignoring the center column if N is odd
		for (int i = 1; i < N - 1; ++i) {
			if (i == N / 2 && N % 2 == 1) {
				continue;
			}

			// bring column from down into front
			exec(4, i, -1);
			exec(0, 0, 2); // bring band from left of Front to right of Front
			exec(4, i, 1); // preserve remaining yellows on Up face
			exec(0, 0, -1); // have band parallel to up

			for (int t = 0; t < 4; ++t) {
				bool which[kCubeWidth];
				for (int j = 0; j < kCubeWidth; ++j)
					which[j] = false;

				int count = 0;
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, i, j) != colorD && get(2, i, j) == colorD) {
						exec(4, j, 1);
						which[j] = true;
						count++;
					}
					else {
						which[j] = false;
					}
				}

				if (count == 0) {
					exec(2, 0, 1);
					continue;
				}
				
				// move band out of the way to Left
				exec(2, i, 1);
				exec(4, 0, 2); // flip upside down
				exec(2, i, -1); // restore other sections
				exec(4, 0, 2);

				for (int j = 1; j < N - 1; ++j) {
					if (which[j]) {
						exec(4, j, -1);
					}
				}
				
				// move band back into front
				exec(2, i, -1);

				exec(2, 0, 1); // turn top to do 4 times
			}

			exec(0, 0, 1); // reverse entrance
			exec(4, i, -1);
			exec(0, 0, 2);
			exec(4, i, 1);
		}

		if (howMany == 2 && numTimes == 0) {
			exec(3, 0, 1);
		}
	}
}

// solves most of the stickers for back face onto the up face
// solves some of left face
// brings back stickers on up face, onto the back face
// must be called if using preliminary0
void Solver::preliminary1() {
	if(kCubeWidth <= 3) {
		return;
	}

	int colorB = storeTopColor;

	// most of back face, but placing on Up face for now, except for center col of odd N
	for (int i = 1; i < N - 1; ++i) {
		if (2 * i + 1 == N) {
			continue;
		}

		exec(4, i, 1); // on Front face, parallel to Left
		exec(0, 0, 2); // on Front face, parallel to Right
		exec(4, i, -1); // restore bottom face

		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, j, N - 1 - i) == colorB && get(0, j, N - 1 - i) != colorB) {
						exec(2, j, -1);
					}
				}
				exec(4, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, j, N - 1 - i) == colorB && get(0, j, N - 1 - i) != colorB) {
						exec(2, j, 1);
					}
				}
				exec(1, 0, 1);
			}
			
		}

		exec(4, i, 1); // undo entrance
		exec(0, 0, 2);
		exec(4, i, -1);
	}

	// access to other two sides for back face that is on Up face
	for (int i = 1; i < N - 1; ++i) {
		exec(0, i, 1); // on Right face, parallel to Front
		exec(1, 0, 2); // on Right face, parallel to Back
		exec(0, i, -1); // restore bottom face

		for (int again = 0; again < 2; ++again) {
			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (2 * j + 1 == N) {
						continue;
					}

					if (get(0, j, N - 1 - i) == colorB && get(1, j, N - 1 - i) != colorB) {
						exec(2, j, -1);
					}
				}
				exec(0, 0, 1);
			}

			for (int numTimes = 0; numTimes < 4; ++numTimes) {
				for (int j = 1; j < N - 1; ++j) {
					if (2 * j + 1 == N) {
						continue;
					}

					if (get(5, j, N - 1 - i) == colorB && get(1, j, N - 1 - i) != colorB) {
						exec(2, j, 1);
					}
				}
				exec(5, 0, 1);
			}
		}

		exec(0, i, 1); // undo entrance
		exec(1, 0, 2);
		exec(0, i, -1);
	}

	// for odd cubes, make sure back center is actually the one we are using
	if (N % 2 == 1 && getFaceColor(5) != storeTopColor) {
		int numTimes = 4;
		while (getFaceColor(5) != storeTopColor && numTimes > 0) {
			exec(2, N / 2, 1);
			numTimes--;
		}
	}

	int positions[kCubeWidth];
	for (int i = 0; i < N; ++i) {
		positions[i] = -1;
	}

	std::map<int, int> myMap;
	myMap[-1] = -1;
	myMap[0] = 0;
	myMap[1] = 1;
	myMap[2] = 5;
	myMap[3] = 4;

	int colorL = getFaceColor(4);

	// build each row of Left on the Front face first as a col
	for (int i = 1; i < N - 1; ++i) {
		if (2 * i + 1 == N) {
			continue;
		}

		for (int j = 1; j < N - 1; ++j) {
			if (get(1, j, i) == colorL) {
				if ((get(0, j, i) != colorL && myMap[positions[j]] != 1) || (get(0, j, i) == colorL && myMap[positions[j]] == 0)) {
					exec(2, j, 1);

					if (myMap[positions[j]] >= 0) {
						positions[j] = (positions[j] + 3) % 4;
					}
				}
			}

		}

		for (int j = 1; j < N - 1; ++j) {
			if (get(4, j, i) == colorL) {
				if ((get(0, j, i) != colorL && myMap[positions[j]] != 4) || (get(0, j, i) == colorL && myMap[positions[j]] == 0)) {
					exec(2, j, -1);

					if (myMap[positions[j]] >= 0) {
						positions[j] = (positions[j] + 1) % 4;
					}
				}
			}
		}

		if (myMap[positions[i]] == 1) {
			exec(2, i, -1);
			positions[i] = 2;
		}

		exec(0, 0, 1);
		exec(2, i, 1);

		if (positions[i] >= 0) {
			positions[i] = (positions[i] + 3) % 4;
		}

		exec(0, 0, -1);

		positions[i] = 3;
	}

	for (int i = 1; i < N - 1; ++i) {
		exec(2, i, positions[i] + 1);
		positions[i] = 3;
	}

	int numCorrect = 0;
	for (int i = 1; i < N - 1; ++i) {
		for (int j = 1; j < N - 1; ++j) {
			if (get(4, i, j) == colorL) {
				numCorrect++;
			}
		}
	}

	// std::cout << "numCorrect0 = " << numCorrect << std::endl;
	
	for (int numTimes = 0; numTimes < 3; ++numTimes) {
		exec(4, 0, 1);

		for (int i = 1; i < N - 1; ++i) {
			if (2 * i + 1 == N) {
				continue;
			}

			exec(2, i, positions[i] + 1);
			positions[i] = 3;

			exec(2, i, -1);
			exec(0, 0, -1);

			for (int j = 1; j < N - 1; ++j) {
				if (get(1, j, i) == colorL) {
					if ((get(0, j, i) != colorL && myMap[positions[j]] != 1) || (get(0, j, i) == colorL && myMap[positions[j]] == 0)) {
						exec(2, j, 1);

						if (myMap[positions[j]] >= 0) {
							positions[j] = (positions[j] + 3) % 4;
						}
					}
				}
			}

			for (int j = 1; j < N - 1; ++j) {
				if (get(4, j, i) == colorL) {
					if ((get(0, j, i) != colorL && myMap[positions[j]] != 4) || (get(0, j, i) == colorL && myMap[positions[j]] == 0)) {
						exec(2, j, -1);

						if (myMap[positions[j]] >= 0) {
							positions[j] = (positions[j] + 1) % 4;
						}
					}
				}
			}

			if (myMap[positions[i]] == 1) {
				exec(2, i, -1);
				positions[i] = 2;
			}

			exec(0, 0, 1);
			exec(2, i, 1);

			if (positions[i] >= 0) {
				positions[i] = (positions[i] + 3) % 4;
			}

			exec(0, 0, -1);

			positions[i] = 3;
		}

		for (int i = 1; i < N - 1; ++i) {
			exec(2, i, positions[i] + 1);
			positions[i] = 3;
		}

		numCorrect = 0;
		for (int i = 1; i < N - 1; ++i) {
			for (int j = 1; j < N - 1; ++j) {
				if (get(4, i, j) == colorL) {
					numCorrect++;
				}
			}
		}

		// std::cout << "numCorrect" << numTimes << " = " << numCorrect << std::endl;
	}
	
	// move the Up face that has Back stickers onto Back face, do this at the end of preliminary

	// for odd cubes, make sure back center is actually the one we are using
	if (N % 2 == 1 && getFaceColor(5) != storeTopColor) {
		int numTimes = 4;
		while (getFaceColor(5) != storeTopColor && numTimes > 0) {
			exec(2, N / 2, 1);
			numTimes--;
		}
	}

	for (int i = 1; i < N / 2; ++i) {
		exec(4, i, -1);
	}
	exec(5, 0, 2);
	for (int i = 1; i < N / 2; ++i) {
		exec(4, i, 2);
	}
	exec(2, 0, 2);
	for (int i = 1; i < N / 2; ++i) {
		exec(4, i, -1);
	}
}

// solving Back face: use swapping method to get down stickers from Left, Right, Up
// then be careful on getting Front to preserve 2 quarter turns per mismatch
// Let M = number of mismatches on this face
// Even Worst-Case: 2M + 144(N-2) + 12 + 46(N-2)
// Odd  Worst-Case: 2M + 144(N-2) + 12 + 46(N-3) + 1 + 22(N-3)
void Solver::solveCenter1() {
	if(kCubeWidth <= 3) {
		return;
	}

	// int colorB = getFaceColor(5);
	int colorB = storeTopColor;

	// get all colorB stickers from left
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(5, i, j) != colorB && get(4, i, j) == colorB) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(2, i, -1);
			exec(4, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, -1);
			}
			exec(4, 0, -1);

			exec(2, i, 1);
			exec(4, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, 1);
			}
			exec(4, 0, -1);

			if (doOther) {
				exec(2, i, -1);
				exec(4, 0, -1);
				exec(2, N - 1 - i, -1);
				exec(4, 0, 1);

				exec(2, i, 1);
				exec(4, 0, -1);
				exec(2, N - 1 - i, 1);
				exec(4, 0, 1);
			}
		}

		exec(4, 0, 1);
	}

	// get all colorB stickers from right
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(5, i, j) != colorB && get(1, i, j) == colorB) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(2, i, 1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, 1);
			}
			exec(1, 0, -1);

			exec(2, i, -1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, -1);
			}
			exec(1, 0, -1);

			if (doOther) {
				exec(2, i, 1);
				exec(1, 0, -1);
				exec(2, N - 1 - i, 1);
				exec(1, 0, 1);

				exec(2, i, -1);
				exec(1, 0, -1);
				exec(2, N - 1 - i, -1);
				exec(1, 0, 1);
			}
		}

		exec(1, 0, 1);
	}

	// get all colorB stickers from up
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(5, j, i) != colorB && get(2, N - 1 - j, N - 1 - i) == colorB) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(1, i, -1);
			exec(2, 0, -1);
			for (int r2 : indices) {
				exec(1, r2, -1);
			}
			exec(2, 0, 1);

			exec(1, i, 1);
			exec(2, 0, -1);
			for (int r2 : indices) {
				exec(1, r2, 1);
			}
			exec(2, 0, 1);

			if (doOther) {
				exec(1, i, -1);
				exec(2, 0, 1);
				exec(1, N - 1 - i, -1);
				exec(2, 0, -1);

				exec(1, i, 1);
				exec(2, 0, 1);
				exec(1, N - 1 - i, 1);
				exec(2, 0, -1);
			}
		}

		exec(2, 0, 1);
	}

	// get all colorB stickers from front
	int howMany = (N % 2 == 1) ? 2 : 1;
	for (int numTimes = 0; numTimes < howMany; ++numTimes) {
		// bring each row from back into left, ignoring the center row if N is odd
		for (int i = 1; i < N - 1; ++i) {
			if (i == N / 2 && N % 2 == 1) {
				continue;
			}

			// bring row from back into left
			exec(2, i, -1);
			exec(4, 0, 2); // bring band from up of Left to down of Left
			exec(2, i, 1); // preserve remaining blue on Front face
			exec(4, 0, -1); // have band parallel to right of Left, aka parallel to Front

			for (int t = 0; t < 4; ++t) {
				bool which[kCubeWidth];
				for (int j = 0; j < kCubeWidth; ++j)
					which[j] = false;

				int count = 0;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, j, N - 1 - i) != colorB && get(0, j, N - 1 - i) == colorB) {
						exec(2, j, 1);
						which[j] = true;
						count++;
					}
					else {
						which[j] = false;
					}
				}

				if (count == 0) {
					exec(0, 0, 1);
					continue;
				}

				// move band out of the way to Up
				exec(0, i, 1);
				exec(2, 0, 2); // flip upside down
				exec(0, i, -1); // restore other sections
				//exec(2, 0, 2);

				for (int j = 1; j < N - 1; ++j) {
					if (which[j]) {
						exec(2, j, -1);
					}
				}

				// move band back into left, but need to preserve bottom, so just undo above
				//exec(2, 0, 2);
				exec(0, i, 1);
				exec(2, 0, 2);
				exec(0, i, -1);

				exec(0, 0, 1); // turn front to do 4 times
			}

			exec(4, 0, 1); // reverse entrance
			exec(2, i, -1);
			exec(4, 0, 2);
			exec(2, i, 1);

		}

		if (howMany == 2 && numTimes == 0) {
			exec(5, 0, 1);
		}
	}
}

// solving Left face: use swapping method to get down stickers from Front, Up
// then be careful on getting Right to preserve 2 quarter turns per mismatch
// Let M = number of mismatches on this face
// Even Worst-Case: 2M + 96(N-2) + 8 + 46(N-2)
// Odd  Worst-Case: 2M + 96(N-2) + 8 + 46(N-3) + 1 + 22(N-3)
void Solver::solveCenter2() {
	if(kCubeWidth <= 3) {
		return;
	}

	int colorL = getFaceColor(4);

	// get all colorB stickers from Front
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(4, i, j) != colorL && get(0, i, j) == colorL) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(2, i, -1);
			exec(0, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, -1);
			}
			exec(0, 0, -1);

			exec(2, i, 1);
			exec(0, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, 1);
			}
			exec(0, 0, -1);

			if (doOther) {
				exec(2, i, -1);
				exec(0, 0, -1);
				exec(2, N - 1 - i, -1);
				exec(0, 0, 1);

				exec(2, i, 1);
				exec(0, 0, -1);
				exec(2, N - 1 - i, 1);
				exec(0, 0, 1);
			}
		}

		exec(0, 0, 1);
	}

	// get all colorB stickers from Up
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(4, j, i) != colorL && get(2, i, N - 1 - j) == colorL) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(0, N - 1 - i, 1);
			exec(2, 0, -1);
			for (int r2 : indices) {
				exec(0, N - 1 - r2, 1);
			}
			exec(2, 0, 1);

			exec(0, N - 1 - i, -1);
			exec(2, 0, -1);
			for (int r2 : indices) {
				exec(0, N - 1 - r2, -1);
			}
			exec(2, 0, 1);

			if (doOther) {
				exec(0, N - 1 - i, 1);
				exec(2, 0, 1);
				exec(0, i, 1);
				exec(2, 0, -1);

				exec(0, N - 1 - i, -1);
				exec(2, 0, 1);
				exec(0, i, -1);
				exec(2, 0, -1);
			}
		}

		exec(2, 0, 1);
	}

	// get all colorB stickers from Right
	int howMany = (N % 2 == 1) ? 2 : 1;
	for (int numTimes = 0; numTimes < howMany; ++numTimes) {
		// bring each row from left into front, ignoring the center row if N is odd
		for (int i = 1; i < N - 1; ++i) {
			if (i == N / 2 && N % 2 == 1) {
				continue;
			}

			// bring row from left into front
			exec(2, i, -1);
			exec(0, 0, 2); // bring band from up of Front to down of Front
			exec(2, i, 1); // preserve remaining red on Right face
			exec(0, 0, -1); // have band parallel to right of Front, aka parallel to Right

			for (int t = 0; t < 4; ++t) {
				bool which[kCubeWidth];
				for (int j = 0; j < kCubeWidth; ++j)
					which[j] = false;

				int count = 0;
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, j, N - 1 - i) != colorL && get(1, j, N - 1 - i) == colorL) {
						exec(2, j, 1);
						which[j] = true;
						count++;
					}
					else {
						which[j] = false;
					}
				}

				if (count == 0) {
					exec(1, 0, 1);
					continue;
				}

				// move band out of the way to Up
				exec(1, i, 1);
				exec(2, 0, 2); // flip upside down
				exec(1, i, -1); // restore other sections
				//exec(2, 0, 2);

				for (int j = 1; j < N - 1; ++j) {
					if (which[j]) {
						exec(2, j, -1);
					}
				}

				// move band back into left, but need to preserve bottom, so just undo above
				//exec(2, 0, 2);
				exec(1, i, 1);
				exec(2, 0, 2);
				exec(1, i, -1);

				exec(1, 0, 1); // turn Right to do 4 times
			}

			exec(0, 0, 1); // reverse entrance
			exec(2, i, -1);
			exec(0, 0, 2);
			exec(2, i, 1);

		}

		if (howMany == 2 && numTimes == 0) {
			exec(4, 0, 1);
		}
	}
}

// solving Up face: use swapping method to get up stickers from Front, Right
// Let M = number of mismatches on this face
// Worst-Case: 2M + 96(N-2) + 8
void Solver::solveCenter3() {
	if(kCubeWidth <= 3) {
		return;
	}

	int colorU = getFaceColor(2);

	// get all colorU stickers from Front
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(2, j, i) != colorU && get(0, j, i) == colorU) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(4, i, 1);
			exec(0, 0, -1);
			for (int r2 : indices) {
				exec(4, r2, 1);
			}
			exec(0, 0, 1);

			exec(4, i, -1);
			exec(0, 0, -1);
			for (int r2 : indices) {
				exec(4, r2, -1);
			}
			exec(0, 0, 1);

			if (doOther) {
				exec(4, i, 1);
				exec(0, 0, 1);
				exec(4, N - 1 - i, 1);
				exec(0, 0, -1);

				exec(4, i, -1);
				exec(0, 0, 1);
				exec(4, N - 1 - i, -1);
				exec(0, 0, -1);
			}
		}

		exec(0, 0, 1);
	}

	// get all colorU stickers from Right
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(2, i, j) != colorU && get(1, j, N - 1 - i) == colorU) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(0, N - 1 - i, 1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(0, N - 1 - r2, 1);
			}
			exec(1, 0, -1);

			exec(0, N - 1 - i, -1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(0, N - 1 - r2, -1);
			}
			exec(1, 0, -1);

			if (doOther) {
				exec(0, N - 1 - i, 1);
				exec(1, 0, -1);
				exec(0, i, 1);
				exec(1, 0, 1);

				exec(0, N - 1 - i, -1);
				exec(1, 0, -1);
				exec(0, i, -1);
				exec(1, 0, 1);
			}
		}

		exec(1, 0, 1);
	}
}

// solving Front and Right faces: use swapping method
// Let M = number of mismatches on this face
// Worst-Case: 2M + 48(N-2) + 4
void Solver::solveLastCenters() {
	if(kCubeWidth <= 3) {
		return;
	}

	int colorF = getFaceColor(0);
	int colorR = getFaceColor(1);

	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		for (int i = 1; i < N - 1; ++i) {
			bool doOther = false;
			std::vector<int> indices;

			for (int j = 1; j < N - 1; ++j) {
				if (get(0, i, j) == colorR && get(1, i, j) == colorF) {
					if (i == j) {
						doOther = true;
					}
					else {
						indices.push_back(j);
					}
				}
			}

			exec(2, i, -1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, -1);
			}
			exec(1, 0, -1);

			exec(2, i, 1);
			exec(1, 0, 1);
			for (int r2 : indices) {
				exec(2, r2, 1);
			}
			exec(1, 0, -1);

			if (doOther) {
				exec(2, i, -1);
				exec(1, 0, -1);
				exec(2, N - 1 - i, -1);
				exec(1, 0, 1);

				exec(2, i, 1);
				exec(1, 0, -1);
				exec(2, N - 1 - i, 1);
				exec(1, 0, 1);
			}
		}

		exec(1, 0, 1);
	}
}

// Solving Centers total (from methods B): Odd: 2 * (# total mismatches) + 775N - 1698 <= 2*5(N-2)^2 + 775N - 1698 = 10N^2 + 735N - 1658
// Even Worst-Case: 2 * (# total mismatches) + 710(N-2) + 48 <= 2*5(N^2) + 710N - 1372 = 10N^2 + 670N - 1332

// Credits to https://ruwix.com/twisty-puzzles/big-cubes-nxnxn-solution/ 
// flips the edge between front and right
// 7 quarter turns
void Solver::flipEdge(int edge) {
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
		exec(0, 0, -1);
		exec(4, 0, -1);
		exec(0, 0, 1);
	}
}

// solve first 4 edges
// Even Worst-Case: 52N - 88, Odd Worst-Case: 52N - 140
void Solver::solveEdges0(std::vector<std::pair<int, int>>& colorPairs) {
	if(kCubeWidth <= 3) {
		return;
	}

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
		if (N % 2 == 1) {
			colorF = get(0, N / 2, N - 1);
			colorR = get(1, N / 2, 0);
		}

		for (int i = 1; i < N - 1; ++i) {
			if (get(0, i, N - 1) == colorF && get(1, i, 0) == colorR) {
				continue;
			}
			// edge between front and up; 5 qt
			if (get(0, 0, i) == colorF && get(2, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, 0, j) == colorF && get(2, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}
				
				for (int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);

				exec(2, 0, 1);
				exec(4, 0, 1);
				for (int j : indices)
					exec(2, j, -1);
			}
			// other one; 7 qt
			else if (get(2, N - 1, N - 1 - i) == colorF && get(0, 0, N - 1 - i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(2, N - 1, N - 1 - j) == colorF && get(0, 0, N - 1 - j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);

				exec(2, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between left and up; 6 qt
			else if (get(4, 0, i) == colorF && get(2, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, 0, j) == colorF && get(2, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(2, 0, 1);
				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);

				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one; 8 qt
			else if (get(2, N - 1 - i, 0) == colorF && get(4, 0, N - 1 - i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(2, N - 1 - j, 0) == colorF && get(4, 0, N - 1 - j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(2, 0, 1);
				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between back and up; 5 qt
			else if (get(5, 0, i) == colorF && get(2, 0, N - 1 - i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, 0, j) == colorF && get(2, 0, N - 1 - j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one; 7 qt
			else if (get(2, 0, i) == colorF && get(5, 0, N - 1 - i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(2, 0, j) == colorF && get(5, 0, N - 1 - j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between right and up; 6 qt
			else if (get(1, 0, i) == colorF && get(2, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, 0, j) == colorF && get(2, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, 2);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one; 8 qt
			else if (get(2, i, N - 1) == colorF && get(1, 0, N - 1 - i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(2, j, N - 1) == colorF && get(1, 0, N - 1 - j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, 2);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between front and left; 10 qt
			else if (get(0, N - 1 - i, 0) == colorF && get(4, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, N - 1 - j, 0) == colorF && get(4, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// other one; 8 qt
			else if (get(4, i, N - 1) == colorF && get(0, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, j, N - 1) == colorF && get(0, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, 1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// edge between left and back; 11 qt
			else if (get(4, N - 1 - i, 0) == colorF && get(5, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, N - 1 - j, 0) == colorF && get(5, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}
				
				exec(5, 0, -1);
				exec(2, 0, 1);
				exec(5, 0, 1);
				exec(2, 0, -1);

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// other one; 9 qt
			else if (get(5, i, N - 1) == colorF && get(4, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, j, N - 1) == colorF && get(4, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(5, 0, -1);
				exec(2, 0, 1);
				exec(5, 0, 1);
				exec(2, 0, -1);

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// edge between right and back; 11 qt
			else if (get(1, i, N - 1) == colorF && get(5, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, j, N - 1) == colorF && get(5, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(5, 0, 1);
				exec(2, 0, -1);
				exec(5, 0, -1);
				exec(2, 0, 1);
				
				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, -1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// other one; 9 qt
			else if (get(5, N - 1 - i, 0) == colorF && get(1, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, N - 1 - j, 0) == colorF && get(1, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				exec(5, 0, 1);
				exec(2, 0, -1);
				exec(5, 0, -1);
				exec(2, 0, 1);

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// edge between down and front; 11 qt
			else if (get(0, N - 1, N - 1 - i) == colorF && get(3, 0, N - 1 - i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, N - 1, N - 1 - j) == colorF && get(3, 0, N - 1 - j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(0, 0, 2);
				exec(2, 0, -1);
				exec(0, 0, -2);
				
				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, 2);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one; 13 qt
			else if (get(3, 0, i) == colorF && get(0, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, 0, j) == colorF && get(0, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(0, 0, 2);
				exec(2, 0, -1);
				exec(0, 0, -2);

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, 2);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between down and left; 10 qt
			else if (get(4, N - 1, N - 1 - i) == colorF && get(3, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, N - 1, N - 1 - j) == colorF && get(3, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(4, 0, 2);
				exec(2, 0, -1);
				exec(4, 0, -2);

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, 1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one, 12 qt
			else if (get(3, N - 1 - i, 0) == colorF && get(4, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, N - 1 - j, 0) == colorF && get(4, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(4, 0, 2);
				exec(2, 0, -1);
				exec(4, 0, -2);

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between down and back; 11 qt
			else if (get(5, N - 1, N - 1 - i) == colorF && get(3, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, N - 1, N - 1 - j) == colorF && get(3, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(5, 0, 2);
				exec(2, 0, 1);
				exec(5, 0, -2);

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, 2);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one; 13 qt
			else if (get(3, N - 1, N - 1 - i) == colorF && get(5, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, N - 1, N - 1 - j) == colorF && get(5, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(5, 0, 2);
				exec(2, 0, 1);
				exec(5, 0, -2);

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, 2);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between down and right; 10 qt
			else if (get(1, N - 1, N - 1 - i) == colorF && get(3, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, N - 1, N - 1 - j) == colorF && get(3, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				exec(1, 0, 2);
				exec(2, 0, 1);
				exec(1, 0, -2);

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, -1);
				exec(2, 0, 1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one; 12 qt
			else if (get(3, i, N - 1) == colorF && get(1, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, j, N - 1) == colorF && get(1, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(1, 0, 2);
				exec(2, 0, 1);
				exec(1, 0, -2);

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// other edge piece on edge between front and right; 12 qt
			else if (get(1, N - 1 - i, 0) == colorF && get(0, N - 1 - i, N - 1) == colorR) {
				
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, N - 1 - j, 0) == colorF && get(0, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, N - 1 - j, 1);
				exec(4, 0, -1);
				exec(2, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, N - 1 - j, -1);

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, 1);
				exec(2, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
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

// solve middle 4 edges
// Even Worst-Case: 108N - 312, Odd Worst-Case: 108N - 420
void Solver::solveEdges1(std::vector<std::pair<int, int>>& colorPairs) {
	if(kCubeWidth <= 3) {
		return;
	}

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
		if (N % 2 == 1) {
			colorF = get(0, N / 2, N - 1);
			colorR = get(1, N / 2, 0);
		}

		for (int i = 1; i < N - 1; ++i) {
			if (get(0, i, N - 1) == colorF && get(1, i, 0) == colorR) {
				continue;
			}
			// edge between front and left; 18 qt
			if (get(0, N - 1 - i, 0) == colorF && get(4, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, N - 1 - j, 0) == colorF && get(4, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -1);
				exec(5, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, 1);
				for(int j : indices)
					exec(2, j, 1);

				exec(1, 0, 1);
				exec(3, 0, 1);
				exec(1, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				exec(3, 0, 2);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			// other one; 20 qt
			else if (get(4, i, N - 1) == colorF && get(0, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, j, N - 1) == colorF && get(0, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				for(int j : indices)
					exec(2, j, 2);
				
				exec(1, 0, 1);
				exec(3, 0, 1);
				exec(1, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				exec(3, 0, 2);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			// edge between left and back; 20 qt
			else if (get(4, N - 1 - i, 0) == colorF && get(5, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, N - 1 - j, 0) == colorF && get(5, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

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
				for(int j : indices)
					exec(2, j, -1);
				exec(5, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, 1);
				for(int j : indices)
					exec(2, j, 1);
				
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			// other one; 22 qt
			else if (get(5, i, N - 1) == colorF && get(4, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, j, N - 1) == colorF && get(4, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

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
				for(int j : indices)
					exec(2, j, -2);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				for(int j : indices)
					exec(2, j, 2);
				
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				exec(3, 0, 1);
			}
			// edge between right and back; 22 qt
			else if (get(1, i, N - 1) == colorF && get(5, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, j, N - 1) == colorF && get(5, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(5, 0, -1);
				exec(3, 0, -1);
				exec(5, 0, 1);
				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, -1);
				exec(3, 0, 2);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -2);

				exec(0, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				exec(3, 0, 1);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(0, 0, 1);
				exec(3, 0, 2);
			}
			// other one; 20 qt
			else if (get(5, N - 1 - i, 0) == colorF && get(1, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, N - 1 - j, 0) == colorF && get(1, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				exec(5, 0, -1);
				exec(3, 0, -1);
				exec(5, 0, 1);
				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, 1);
				exec(3, 0, 2);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -1);

				exec(0, 0, -1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				exec(3, 0, 1);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(0, 0, 1);
				exec(3, 0, 2);
			}
			// edge between down and front; 8 qt
			else if (get(0, N - 1, N - 1 - i) == colorF && get(3, 0, N - 1 - i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(0, N - 1, N - 1 - j) == colorF && get(3, 0, N - 1 - j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -1);
				
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
			}
			// other one; 10 qt
			else if (get(3, 0, i) == colorF && get(0, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, 0, j) == colorF && get(0, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -2);
				
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
			}
			
			// edge between down and left; 10 qt
			else if (get(4, N - 1, N - 1 - i) == colorF && get(3, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(4, N - 1, N - 1 - j) == colorF && get(3, j, 0) == colorR) {
						indices.push_back(j);
					}
				}

				exec(3, 0, 1);

				// do down + front
				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, 1);
				exec(3, 0, -1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -1);

				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(3, 0, -1);
			}
			// other one; 12 qt
			else if (get(3, N - 1 - i, 0) == colorF && get(4, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, N - 1 - j, 0) == colorF && get(4, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				exec(3, 0, 1);

				// do down + front
				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, -1);
				exec(3, 0, -1);
				exec(4, 0, 1);
				for(int j : indices)
					exec(2, j, -2);
				
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(3, 0, -1);
			}
			// edge between down and back; 6 qt
			else if (get(5, N - 1, N - 1 - i) == colorF && get(3, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(5, N - 1, N - 1 - j) == colorF && get(3, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 1);
				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				exec(3, 0, -1);
				for(int j : indices)
					exec(2, j, -1);
			}
			// other one; 8 qt
			else if (get(3, N - 1, N - 1 - i) == colorF && get(5, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, N - 1, N - 1 - j) == colorF && get(5, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 2);
				exec(4, 0, -1);
				exec(3, 0, 1);
				exec(4, 0, 1);
				exec(3, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// edge between down and right; 8 qt
			else if (get(1, N - 1, N - 1 - i) == colorF && get(3, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(1, N - 1, N - 1 - j) == colorF && get(3, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, 2);
				exec(5, 0, 1);
				exec(3, 0, 1);
				exec(5, 0, -1);
				exec(3, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
			}
			// other one; 6 qt
			else if (get(3, i, N - 1) == colorF && get(1, N - 1, i) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N - 1; ++j) {
					if (get(3, j, N - 1) == colorF && get(1, N - 1, j) == colorR) {
						indices.push_back(j);
					}
				}

				for(int j : indices)
					exec(2, j, -1);
				exec(5, 0, -1);
				exec(3, 0, 1);
				exec(5, 0, 1);
				exec(3, 0, -1);
				for(int j : indices)
					exec(2, j, 1);
			}
			// other edge piece on edge between front and right; 32 qt
			else if (get(1, N - 1 - i, 0) == colorF && get(0, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = 1; j < N / 2; ++j) {
					if (get(1, N - 1 - j, 0) == colorF && get(0, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}

				for (int j : indices)
					exec(2, N - 1 - j, 1);
				for (int j : indices)
					exec(2, j, 1);
				exec(4, 0, 2);
				for (int j : indices)
					exec(2, j, -2);
				for (int j : indices)
					exec(2, N - 1 - j, -2);
				exec(4, 0, -2);
				for (int j : indices)
					exec(2, j, 1);
				for (int j : indices)
					exec(2, N - 1 - j, 1);

				// now handle using front + left

				exec(4, 0, 1);
				exec(3, 0, 1);
				exec(4, 0, -1);
				for(int j : indices)
					exec(2, j, -2);
				exec(5, 0, 1);
				exec(3, 0, 2);
				exec(5, 0, -1);
				for(int j : indices)
					exec(2, j, 2);

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

// solve last 4 edges
// Even Worst-Case: 66.5N - 125, Odd Worst-Case: 66.5N - 191.5
void Solver::solveEdges2() {
	if(kCubeWidth <= 3) {
		return;
	}

	std::map<int, char> myMap;
	myMap[0] = 'G';
	myMap[1] = 'R';
	myMap[2] = 'W';
	myMap[3] = 'Y';
	myMap[4] = 'O';
	myMap[5] = 'B';

	// build edge on edge between Front and Right face
	// top edges, bottom edges are all solved
	for (int numTimes = 0; numTimes < 3; numTimes++) {
		int colorF = get(0, N / 2, N - 1);
		int colorR = get(1, N / 2, 0);

		for (int i = 1; i < N - 1; ++i) {
			int lowLimit = 1;
			int highLimit = N / 2;

			if (i >= N / 2) {
				lowLimit = N / 2;
				highLimit = N - 1;
			}

			// correct piece already
			if (get(0, i, N - 1) == colorF && get(1, i, 0) == colorR) {

			}
			// if desired piece on opposite side of same edge; 11 qt
			else if (get(0, N - 1 - i, N - 1) == colorR && get(1, N - 1 - i, 0) == colorF) {
				std::vector<int> indices;
				for (int j = lowLimit; j < highLimit; ++j) {
					if (get(0, N - 1 - j, N - 1) == colorR && get(1, N - 1 - j, 0) == colorF) {
						indices.push_back(j);
					}
				}
				
				for(int j : indices)
					exec(2, j, -1);
				for(int j : indices)
					exec(2, N - 1 - j, -1);
				flipEdge(1 + 5); // right + back
				for(int j : indices)
					exec(2, j, 1);
				for(int j : indices)
					exec(2, N - 1 - j, 1);
			}
			// on edge between back and right; 9 qt
			else if (get(5, N - 1 - i, 0) == colorF && get(1, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = lowLimit; j < highLimit; ++j) {
					if (get(5, N - 1 - j, 0) == colorF && get(1, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}
				
				for(int j : indices)
					exec(2, j, -1);
				flipEdge(1 + 5); // right + back
				for(int j : indices)
					exec(2, j, 1);
			}
			// reverse that; 16 qt
			else if (get(1, i, N - 1) == colorF && get(5, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = lowLimit; j < highLimit; ++j) {
					if (get(1, j, N - 1) == colorF && get(5, j, 0) == colorR) {
						indices.push_back(j);
					}
				}
				
				flipEdge(1 + 5); // right + back
				for(int j : indices)
					exec(2, j, -1);
				flipEdge(1 + 5);
				for(int j : indices)
					exec(2, j, 1);
			}
			// on edge between front and left; 9 qt
			else if (get(0, N - 1 - i, 0) == colorF && get(4, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = lowLimit; j < highLimit; ++j) {
					if (get(0, N - 1 - j, 0) == colorF && get(4, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}
				
				for(int j : indices)
					exec(2, j, 1);
				flipEdge(4 + 0); // left + front
				for(int j : indices)
					exec(2, j, -1);
			}
			// reverse that; 16 qt
			else if (get(4, i, N - 1) == colorF && get(0, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = lowLimit; j < highLimit; ++j) {
					if (get(4, j, N - 1) == colorF && get(0, j, 0) == colorR) {
						indices.push_back(j);
					}
				}
				
				flipEdge(4 + 0); // left + front
				for(int j : indices)
					exec(2, j, 1);
				flipEdge(4 + 0);
				for(int j : indices)
					exec(2, j, -1);
			}
			// on edge between left and back; 11 qt
			else if (get(4, N - 1 - i, 0) == colorF && get(5, N - 1 - i, N - 1) == colorR) {
				std::vector<int> indices;
				for (int j = lowLimit; j < highLimit; ++j) {
					if (get(4, N - 1 - j, 0) == colorF && get(5, N - 1 - j, N - 1) == colorR) {
						indices.push_back(j);
					}
				}
				
				for(int j : indices)
					exec(2, j, 2);
				flipEdge(5 + 4); // back + left
				for(int j : indices)
					exec(2, j, -2);
			}
			// reverse that; 18 qt
			else if (get(5, i, N - 1) == colorF && get(4, i, 0) == colorR) {
				std::vector<int> indices;
				for (int j = lowLimit; j < highLimit; ++j) {
					if (get(5, j, N - 1) == colorF && get(4, j, 0) == colorR) {
						indices.push_back(j);
					}
				}
				
				flipEdge(5 + 4);
				for(int j : indices)
					exec(2, j, 2);
				flipEdge(5 + 4);
				for(int j : indices)
					exec(2, j, -2);
			}
		}

		if (numTimes == 0) {
			// bring 1st finished edge to edge between back + left
			exec(1, 0, 2);
			exec(5, 0, 2);
		}
		else if (numTimes == 1) {
			// bring 2nd finished edge to edge between front + left
			exec(0, 0, 2);
		}
		else if (numTimes == 2) {
			// bring 3rd finished edge to edge between right + back
			exec(1, 0, 2);
		}
	}

	// build last edge, on edge between front + right
	int colorF = get(0, N / 2, N - 1);
	int colorR = get(1, N / 2, 0);

	for (int i = 1; i < N - 1; ++i) {
		// correct piece already
		if (get(0, i, N - 1) == colorF && get(1, i, 0) == colorR) {
			continue;
		}
		// if desired piece on opposite side of same edge; 25 qt
		// Red Bull Algorithm - https://www.youtube.com/watch?v=knMCvKdJFgk
		else if (get(0, N - 1 - i, N - 1) == colorR && get(1, N - 1 - i, 0) == colorF) {
			std::vector<int> indices;
			for (int j = 1; j < N / 2; ++j) {
				if (get(0, N - 1 - j, N - 1) == colorR && get(1, N - 1 - j, 0) == colorF) {
					indices.push_back(j);
				}
			}

			for(int j : indices)
				exec(2, N - 1 - j, 2);
			exec(5, 0, 2);
			exec(1, 0, 2);
			for(int j : indices)
				exec(2, j, 1);
			exec(1, 0, 2);
			
			for(int j : indices)
				exec(2, N - 1 - j, 1);
			exec(1, 0, 2);
			for(int j : indices)
				exec(2, N - 1 - j, -1);
			exec(1, 0, 2);
			exec(0, 0, 2);
			for(int j : indices)
				exec(2, N - 1 - j, -1);
			exec(0, 0, 2);
			
			for(int j : indices)
				exec(2, j, -1);
			exec(5, 0, 2);
			for(int j : indices)
				exec(2, N - 1 - j, 2);
		}
	}
}

// wrapper function that calls solveEdges0, solveEdges1, solveEdges2
// Total Edges: Even Worst-Case: 226.5N - 525, Odd Worst-Case: 226.5N - 751.5
void Solver::solveEdges() {
	if (kCubeWidth <= 3) {
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

// first step in solving 3x3x3: make bottom cross
void Solver::solveCross() {
	// edges only exist for N >= 3
	if (kCubeWidth <= 2) {
		return;
	}

	std::vector<int> sideColors{ getFaceColor(0), getFaceColor(1), getFaceColor(5), getFaceColor(4) };
	
	int colorDown = getFaceColor(3);
	
	// bring each edge onto the edge between front and down, then rotate bottom counterclockwise
	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		int colorSide = sideColors[numTimes];

		// already solved
		if (get(3, 0, 1) == colorDown && get(0, N - 1, 1) == colorSide) {

		}
		// just backwards
		else if (get(0, N - 1, 1) == colorDown && get(3, 0, 1) == colorSide) {
			exec(0, 0, -1);
			exec(1, 0, 1);
			exec(2, 0, 1);
			exec(1, 0, -1);
			exec(0, 0, 2);
		}
		// desired edge on edge between right and down
		else if (get(3, 1, N - 1) == colorDown && get(1, N - 1, 1) == colorSide) {
			exec(1, 0, 2);
			exec(2, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(1, N - 1, 1) == colorDown && get(3, 1, N - 1) == colorSide) {
			exec(1, 0, 1);
			exec(0, 0, 1);
		}
		// desired edge on edge between back and down
		else if (get(3, N - 1, N - 2) == colorDown && get(5, N - 1, 1) == colorSide) {
			exec(5, 0, 2);
			exec(2, 0, 2);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(5, N - 1, 1) == colorDown && get(3, N - 1, N - 2) == colorSide) {
			exec(5, 0, 1);
			exec(1, 0, -1);
			exec(2, 0, 1);
			exec(1, 0, 1);
			exec(0, 0, 2);
		}
		// desired edge on edge between left and down
		else if (get(3, 1, 0) == colorDown && get(4, N - 1, N - 2) == colorSide) {
			exec(3, 0, 1);
		}
		// flipped other way
		else if (get(4, N - 1, N - 2) == colorDown && get(3, 1, 0) == colorSide) {
			exec(4, 0, -1);
			exec(0, 0, -1);
			exec(4, 0, 1);
		}
		// edge between front and right
		else if (get(1, 1, 0) == colorDown && get(0, 1, N - 1) == colorSide) {
			exec(0, 0, 1);
		}
		// flipped other way
		else if (get(0, 1, N - 1) == colorDown && get(1, 1, 0) == colorSide) {
			exec(1, 0, 1);
			exec(2, 0, 1);
			exec(1, 0, -1);
			exec(0, 0, 2);
		}
		// edge between right and back
		else if (get(5, 1, 0) == colorDown && get(1, 1, N - 1) == colorSide) {
			exec(1, 0, -1);
			exec(2, 0, 1);
			exec(1, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(1, 1, N - 1) == colorDown && get(5, 1, 0) == colorSide) {
			exec(5, 0, 1);
			exec(2, 0, 2);
			exec(5, 0, -1);
			exec(0, 0, 2);
		}
		// edge between back + left
		else if (get(4, 1, 0) == colorDown && get(5, 1, N - 1) == colorSide) {
			exec(5, 0, -1);
			exec(2, 0, 2);
			exec(5, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(5, 1, N - 1) == colorDown && get(4, 1, 0) == colorSide) {
			exec(4, 0, 1);
			exec(2, 0, -1);
			exec(4, 0, -1);
			exec(0, 0, 2);
		}
		// edge between left + front
		else if (get(0, 1, 0) == colorDown && get(4, 1, N - 1) == colorSide) {
			exec(4, 0, -1);
			exec(2, 0, -1);
			exec(4, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(4, 1, N - 1) == colorDown && get(0, 1, 0) == colorSide) {
			exec(0, 0, -1);
		}
		// edge between front + up
		else if (faces[2][N - 1][1] == colorDown && faces[0][0][1] == colorSide) {
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(0, 0, 1) == colorDown && get(2, N - 1, 1) == colorSide) {
			exec(2, 0, -1);
			exec(1, 0, -1);
			exec(0, 0, 1);
			exec(1, 0, 1);
		}
		// edge between right + up
		else if (get(2, N - 2, N - 1) == colorDown && get(1, 0, 1) == colorSide) {
			exec(2, 0, 1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(1, 0, 1) == colorDown && get(2, N - 2, N - 1) == colorSide) {
			exec(1, 0, -1);
			exec(0, 0, 1);
			exec(1, 0, 1);
		}
		// edge between back + up
		else if (get(2, 0, 1) == colorDown && get(5, 0, 1) == colorSide) {
			exec(2, 0, 2);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(5, 0, 1) == colorDown && get(2, 0, 1) == colorSide) {
			exec(2, 0, 1);
			exec(1, 0, -1);
			exec(0, 0, 1);
			exec(1, 0, 1);
		}
		// edge between left + up
		else if (get(2, 1, 0) == colorDown && get(4, 0, 1) == colorSide) {
			exec(2, 0, -1);
			exec(0, 0, 2);
		}
		// flipped other way
		else if (get(4, 0, 1) == colorDown && get(2, 1, 0) == colorSide) {
			exec(4, 0, 1);
			exec(0, 0, -1);
			exec(4, 0, -1);
		}

		// once done, move finished edge to another edge on bottom
		exec(3, 0, -1);
	}
}

// next step in solving 3x3x3: get corners of bottom layer, so all of bottom layer complete
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

		// corner meeting front, right, down
		std::vector<int> corner{ get(0, N - 1, N - 1), get(1, N - 1, 0), get(3, 0, N - 1) };
		std::sort(corner.begin(), corner.end());
		if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {
			
			// if cubie in right place
			if (get(3, 0, N - 1) == colorDown) {
				// already in place
			}
			// if down color on front face
			else if (get(0, N - 1, N - 1) == colorDown) {
				exec(0, 0, -1);
				exec(2, 0, -1);
				exec(0, 0, 1);
				exec(2, 0, 2);
				exec(1, 0, 1);
				exec(2, 0, -1);
				exec(1, 0, -1);
			}
			// if down color on right face
			else if (get(1, N - 1, 0) == colorDown) {
				exec(1, 0, 1);
				exec(2, 0, 1);
				exec(1, 0, -1);
				exec(2, 0, -2);
				exec(0, 0, -1);
				exec(2, 0, 1);
				exec(0, 0, 1);
			}

		}
		else {
			// corner meeting front, left, down
			corner[0] = get(0, N - 1, 0);
			corner[1] = get(4, N - 1, N - 1);
			corner[2] = get(3, 0, 0);
			std::sort(corner.begin(), corner.end());

			if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {

				// if down color on down face
				if (get(3, 0, 0) == colorDown) {
					exec(4, 0, -1);
					exec(2, 0, -2);
					exec(4, 0, 1);
					exec(0, 0, -1);
					exec(2, 0, 1);
					exec(0, 0, 1);
				}
				// if down color on front face
				else if (get(0, N - 1, 0) == colorDown) {
					exec(0, 0, 1);
					exec(2, 0, -2);
					exec(0, 0, -2);
					exec(2, 0, 1);
					exec(0, 0, 1);
				}
				// if down color on left face
				else if (get(4, N - 1, N - 1) == colorDown) {
					exec(4, 0, -1);
					exec(2, 0, -1);
					exec(4, 0, 1);
					exec(2, 0, 1);
					exec(1, 0, 1);
					exec(2, 0, -1);
					exec(1, 0, -1);
				}

			}
			else {
				// corner meeting left, down, back
				corner[0] = get(4, N - 1, 0);
				corner[1] = get(3, N - 1, 0);
				corner[2] = get(5, N - 1, N - 1);
				std::sort(corner.begin(), corner.end());

				if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {

					// if down color on down face
					if (get(3, N - 1, 0) == colorDown) {
						exec(5, 0, -1);
						exec(2, 0, -1);
						exec(5, 0, 1);
						exec(2, 0, 2);
						exec(0, 0, -1);
						exec(2, 0, 1);
						exec(0, 0, 1);
					}
					// if down color on left face
					else if (get(4, N - 1, 0) == colorDown) {
						exec(4, 0, 1);
						exec(2, 0, 1);
						exec(4, 0, -1);
						exec(0, 0, -1);
						exec(2, 0, 1);
						exec(0, 0, 1);
					}
					// if down color on back face
					else if (get(5, N - 1, N - 1) == colorDown) {
						exec(5, 0, -1);
						exec(2, 0, -1);
						exec(5, 0, 1);
						exec(1, 0, 1);
						exec(2, 0, -1);
						exec(1, 0, -1);
					}

				}
				else {
					// corner meeting right, back, down
					corner[0] = get(1, N - 1, N - 1);
					corner[1] = get(5, N - 1, 0);
					corner[2] = get(3, N - 1, N - 1);
					std::sort(corner.begin(), corner.end());

					if (desired[0] == corner[0] && desired[1] == corner[1] && desired[2] == corner[2]) {

						// if down color on down face
						if (get(3, N - 1, N - 1) == colorDown) {
							exec(5, 0, 1);
							exec(2, 0, 2);
							exec(5, 0, -1);
							exec(1, 0, 1);
							exec(2, 0, -1);
							exec(1, 0, -1);
						}
						// if down color on right face
						else if (get(1, N - 1, N - 1) == colorDown) {
							exec(1, 0, -1);
							exec(2, 0, 2);
							exec(1, 0, 2);
							exec(2, 0, -1);
							exec(1, 0, -1);
						}
						// if down color on back face
						else if (get(5, N - 1, 0) == colorDown) {
							exec(5, 0, 1);
							exec(2, 0, 1);
							exec(5, 0, -1);
							exec(2, 0, -1);
							exec(0, 0, -1);
							exec(2, 0, 1);
							exec(0, 0, 1);
						}

					}
					else {
						// corner meeting front, right, up
						do {
							exec(2, 0, 1);
							corner[0] = get(0, 0, N - 1);
							corner[1] = get(1, 0, 0);
							corner[2] = get(2, N - 1, N - 1);
							std::sort(corner.begin(), corner.end());
						} while (desired[0] != corner[0] || desired[1] != corner[1] || desired[2] != corner[2]);

						// if down color on up face
						if (get(2, N - 1, N - 1) == colorDown) {
							exec(1, 0, 1);
							exec(2, 0, -1);
							exec(1, 0, -1);
							exec(0, 0, -1);
							exec(2, 0, 2);
							exec(0, 0, 1);
						}
						// if down color on right face
						else if (get(1, 0, 0) == colorDown) {
							exec(2, 0, -1);
							exec(0, 0, -1);
							exec(2, 0, 1);
							exec(0, 0, 1);
						}
						// if down color on front face
						else if (get(0, 0, N - 1) == colorDown) {
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

// next step in solving 3x3x3: solve the edges between down and up faces
void Solver::solveSecondLayer() {
	if (kCubeWidth <= 2) {
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
		if (get(front, 1, N - 1) == colorFront && get(right, 1, 0) == colorRight) {
			continue;
		}
		// edge flipped around
		else if(get(right, 1, 0) == colorFront && get(front, 1, N - 1) == colorRight) {
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
		else if (get(left, 1, N - 1) == colorFront && get(front, 1, 0) == colorRight) {
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
		else if (get(front, 1, 0) == colorFront && get(left, 1, N - 1) == colorRight) {
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
		else if (get(back, 1, N - 1) == colorFront && get(left, 1, 0) == colorRight) {
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
		else if (get(left, 1, 0) == colorFront && get(back, 1, N - 1) == colorRight) {
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
		else if (get(back, 1, 0) == colorFront && get(right, 1, N - 1) == colorRight) {
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
		else if (get(right, 1, N - 1) == colorFront && get(back, 1, 0) == colorRight) {
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
			// need to access correct sticker on Up face, depending on which edge it is solving right now
			// need to take correct sticker on Up, starting with if our relative Front is absolute Front, absolute Right, abs Back, or abs Left
			std::vector<int> topRotationRow{ N - 1, 1, 0, 1 };
			std::vector<int> topRotationCol{ 1, N - 1, 1, 0 };

			int topRow = topRotationRow[numTimes];
			int topCol = topRotationCol[numTimes];

			std::vector<int> desired{ colorFront, colorRight };
			std::sort(desired.begin(), desired.end());

			std::vector<int> myEdge;

			// turn the Up face until the edge you need is in relative position front + up
			do {
				exec(up, 0, 1);
				myEdge.clear();
				myEdge.push_back(get(front, 0, 1));
				myEdge.push_back(get(up, topRow, topCol));
				std::sort(myEdge.begin(), myEdge.end());
			} while (desired[0] != myEdge[0] || desired[1] != myEdge[1]);

			// now that our desired piece is in the edge between Up face and relative Front face, handle appropriately, depending on orientation
			if (get(front, 0, 1) == colorFront) {
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

// solve up layer cross
void Solver::solveLastCross() {
	// small enough cubes do not have edges
	if (kCubeWidth <= 2) {
		return;
	}

	int colorTop = getFaceColor(2);

	bool front = get(2, N - 1, 1) == colorTop;
	bool right = get(2, 1, N - 1) == colorTop;
	bool back = get(2, 0, 1) == colorTop;
	bool left = get(2, 1, 0) == colorTop;

	int count = front + right + back + left;

	// if incorrect amount, fix parity
	if (N % 2 == 0 && count % 2 == 1) {
		for (int i = 0; i < 4; ++i) {
			if (get(2, N - 1, 1) == colorTop) {
				fixParityEdge();
			}
			exec(2, 0, 1);
		}

		front = get(2, N - 1, 1) == colorTop;
		right = get(2, 1, N - 1) == colorTop;
		back = get(2, 0, 1) == colorTop;
		left = get(2, 1, 0) == colorTop;

		count = front + right + back + left;
	}

	while (count < 4) {
		if (count >= 2) {
			while (!(left && back) && !(left && right)) {
				exec(2, 0, 1);

				front = get(2, N - 1, 1) == colorTop;
				right = get(2, 1, N - 1) == colorTop;
				back = get(2, 0, 1) == colorTop;
				left = get(2, 1, 0) == colorTop;
			}
		}

		exec(0, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(1, 0, -1);
		exec(0, 0, -1);

		front = get(2, N - 1, 1) == colorTop;
		right = get(2, 1, N - 1) == colorTop;
		back = get(2, 0, 1) == colorTop;
		left = get(2, 1, 0) == colorTop;

		count = front + right + back + left;
	}
}

// get correct edge pieces on up layer in the right place and orientation
void Solver::solveLastEdges() {
	// small enough cubes do not have edges
	if (kCubeWidth <= 2) {
		return;
	}

	int colorUp = getFaceColor(2);
	int colorBack = getFaceColor(5);
	int colorFront = getFaceColor(0);
	int colorLeft = getFaceColor(4);
	int colorRight = getFaceColor(1);

	// orient last layer so that back face edge has correct color
	while (get(5, 0, 1) != colorBack) {
		exec(2, 0, 1);
	}

	// 6 possible ways that the other 3 edges are positioned; one of them is solved state
	// from front view, we want Orange, Green, Red on Left, Front, Right
	// could have (Green, Orange, Red)
	if (get(4, 0, 1) == colorFront && get(0, 0, 1) == colorLeft) {
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
	else if (get(4, 0, 1) == colorRight && get(1, 0, 1) == colorLeft) {
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
	else if (get(0, 0, 1) == colorRight && get(1, 0, 1) == colorFront) {
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
	else if (get(4, 0, 1) == colorFront && get(0, 0, 1) == colorRight && get(1, 0, 1) == colorLeft) {
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
	else if (get(4, 0, 1) == colorRight && get(0, 0, 1) == colorLeft && get(1, 0, 1) == colorFront) {
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

// place all corners of up layer into the correct spot (not necessarily correct orientation)
void Solver::solveLastCornerPosition() {
	int colorFront = getFaceColor(0);
	int colorRight = getFaceColor(1);
	int colorUp = getFaceColor(2);
	int colorDown = getFaceColor(3);
	int colorLeft = getFaceColor(4);
	int colorBack = getFaceColor(5);
	
	// front-right-up, front-left-up, back-left-up, back-right-up
	int corners[12] = { get(0, 0, N - 1), get(1, 0, 0), get(2, N - 1, N - 1), 
						get(0, 0, 0), get(4, 0, N - 1), get(2, N - 1, 0), 
						get(5, 0, N - 1), get(4, 0, 0), get(2, 0, 0), 
						get(5, 0, 0), get(1, 0, N - 1), get(2, 0, N - 1) };

	for (int i = 0; i < 4; ++i) {
		std::sort(corners + 3 * i, corners + 3 * i + 3);
	}

	int desired[12] = { colorFront, colorRight, colorUp, colorFront, colorLeft, colorUp, 
		colorBack, colorLeft, colorUp, colorBack, colorRight, colorUp };

	for (int i = 0; i < 4; ++i) {
		std::sort(desired + 3 * i, desired + 3 * i + 3);
	}

	int indices[4] = { 0, 0, 0, 0 };
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (desired[3 * j] == corners[3 * i] && desired[3 * j + 1] == corners[3 * i + 1] && desired[3 * j + 2] == corners[3 * i + 2]) {
				indices[i] = j + 1;
				break;
			}
		}
	}

	int permutation = 1000 * indices[0] + 100 * indices[1] + 10 * indices[2] + indices[3];
	// std::cout << "PPPPPPPPERMUTATION = " << permutation << std::endl;

	if (permutation == 1234) {

	}
	else if (permutation == 1243) {
		exec(2, 0, 2);
		fixParityAdjacentCorners();
		exec(2, 0, 2);
	}
	else if (permutation == 1324) {
		exec(2, 0, -1);
		fixParityAdjacentCorners();
		exec(2, 0, 1);
	}
	else if (permutation == 1342) {
		// turn 234 clockwise
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
	}
	else if (permutation == 1423) {
		// turn 234 counter-clockwise
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);
	}
	else if (permutation == 1432) {
		exec(2, 0, 1);
		fixParityOppositeCorners();
		exec(2, 0, -1);
	}
	else if (permutation == 2134) {
		fixParityAdjacentCorners();
	}
	else if (permutation == 2143) {
		exec(2, 0, 1);

		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);

		exec(2, 0, -1);

		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);
	}
	else if (permutation == 2314) {
		exec(2, 0, 1);

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);

		exec(2, 0, -1);
	}
	else if (permutation == 2341) {
		exec(2, 0, 1);
		fixParityAdjacentCorners();
		exec(2, 0, -1);

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
	}
	else if (permutation == 2413) {
		fixParityOppositeCorners();

		//ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);
	}
	else if (permutation == 2431) {
		exec(2, 0, 2);

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);

		exec(2, 0, -2);
	}
	else if (permutation == 3124) {
		exec(2, 0, 1);

		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);

		exec(2, 0, -1);
	}
	else if (permutation == 3142) {
		fixParityAdjacentCorners();

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
	}
	else if (permutation == 3214) {
		fixParityOppositeCorners();
	}
	else if (permutation == 3241) {
		exec(2, 0, -1);
		
		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);

		exec(2, 0, 1);
	}
	else if (permutation == 3412) {
		exec(2, 0, -1);
		
		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);

		exec(2, 0, 1);

		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);
	}
	else if (permutation == 3421) {
		exec(2, 0, 1);
		fixParityOppositeCorners();
		exec(2, 0, -1);

		exec(2, 0, 1);
		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		
		exec(2, 0, -1);
	}
	else if (permutation == 4123) {
		exec(2, 0, 1);
		fixParityAdjacentCorners();
		exec(2, 0, -1);

		exec(2, 0, 1);
		//ccw 
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);

		exec(2, 0, -1);
	}
	else if (permutation == 4132) {
		exec(2, 0, 1);
		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);

		exec(2, 0, -1);

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
	}
	else if (permutation == 4213) {
		exec(2, 0, 1);

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);

		exec(2, 0, -1);

		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);
	}
	else if (permutation == 4231) {
		exec(2, 0, 1);
		fixParityAdjacentCorners();
		exec(2, 0, -1);
	}
	else if (permutation == 4312) {
		fixParityOppositeCorners();

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
	}
	else if (permutation == 4321) {
		exec(2, 0, 2);

		// cw
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, 1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);

		exec(2, 0, -2);

		// ccw
		exec(2, 0, 1);
		exec(1, 0, 1);
		exec(2, 0, -1);
		exec(4, 0, -1);
		exec(2, 0, 1);
		exec(1, 0, -1);
		exec(2, 0, -1);
		exec(4, 0, 1);
	}
}

// assume corners are in the correct position; now orient each small cube so correct sticker on top
void Solver::solveLastCornerOrientation() {
	int colorUp = getFaceColor(2);

	for (int numTimes = 0; numTimes < 4; ++numTimes) {
		while (get(2, N - 1, N - 1) != colorUp) {
			exec(1, 0, -1);
			exec(3, 0, -1);
			exec(1, 0, 1);
			exec(3, 0, 1);
		}

		exec(2, 0, 1);
	}
}

// wrapper function that calls all of the 3x3x3 helper functions above, to solve a 3x3x3 cube
// https://ruwix.com/the-rubiks-cube/how-to-solve-the-rubiks-cube-beginners-method/ for reference
void Solver::solve3x3x3() {
	solveCross();
	solveCorners4();
	solveSecondLayer();
	solveLastCross();
	solveLastEdges();
	solveLastCornerPosition();
	solveLastCornerOrientation();
}

// wrapper function that calls all helper functions to solve cube
// Warning: this might make deque super big with lots of moves, for large N
void Solver::solve() {
	preliminary0();
	solveCenter0();
	preliminary1();
	
	solveCenter1();
	solveCenter2();
	solveCenter3();
	solveLastCenters();
	
	solveEdges();
	solve3x3x3();
}