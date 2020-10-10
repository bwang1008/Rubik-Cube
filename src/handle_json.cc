#include "handle_json.h"

#include <fstream>				// read in json file
#include <iostream>				// std::cerr, std::endl
#include <map>					// map from characters in json to int (index)

#include "config.h"				// compare constants N and kCubeWidth
#include "json.hpp"				// json handling

// used tutorial at http://rachelnertia.github.io/programming/2016/07/03/json-hpp/
/**
 *	json_path is a string saying where the JSON file is located in file system
 *	solver is a Solver object that has a container for all sticker colors; we fill it here
 *	colors will contain 6 rgb values for the 6 colors of stickers; we fill it here
 *  return int: -1 means error, 0 means success
*/
int read_json(std::string& json_path, Solver* solver, std::vector<glm::uvec3>& colors) {
	std::ifstream reader(json_path);
	if(!reader.is_open()) {
		std::cerr << "Unable to open file " << json_path << std::endl;
		return -1;
	}

	std::cout << "Found file!" << std::endl;

	using json = nlohmann::json;
	json obj;

	reader >> obj; 					// read in actual object
	reader.close();

	std::cout << "Read file!" << std::endl;

	if(obj.find("N") != obj.end() && obj["N"] != kCubeWidth) {
		std::cerr << "Mismatch between kCubeWidth = " << kCubeWidth << " and file N = " << obj["N"] << std::endl;
		std::cerr << "Please change kCubeWidth = " << obj["N"] << " in config.h" << std::endl;
		return -1;
	}

	std::string faces[6] = {"front", "right", "up", "down", "left", "back"};

	for(int i = 0; i < 6; ++i) {
		if(obj.find(faces[i]) == obj.end()) {
			std::cerr << "Could not find data \"" << faces[i]<< "\"" << std::endl;
			return -1;
		}
	}

	std::cout << "gonna fill data[][][]" << std::endl;

	char data[6][kCubeWidth][kCubeWidth];
	std::map<char, int> counter;

	for(int i = 0; i < 6; ++i) {
		json face_data = obj[faces[i]];

		int count = 0;
		for(const std::string& line : face_data) {
			if(count == kCubeWidth) {
				std::cerr << "Encountered more than " << kCubeWidth << " lines in " << faces[i] << std::endl;
				return -1;
			}

			if(line.size() != kCubeWidth) {
				std::cerr << faces[i] << " line " << count << " has " << line.size() << " chars rather than " << kCubeWidth << std::endl;
				return -1;
			}

			for(int j = 0; j < kCubeWidth; ++j) {
				char c = line[j];
				data[i][count][j] = c;

				++counter[c];
			}

			count++;
		}

		if(count < kCubeWidth) {
			std::cerr << "Encountered less than " << kCubeWidth << " lines in " << faces[i] << std::endl;
			return -1;
		}
	}

	std::vector<char> chars;
	std::vector<int> counts;

	for(std::map<char, int>::iterator iter = counter.begin(); iter != counter.end(); ++iter) {
		chars.push_back(iter->first);
		counts.push_back(iter->second);
	}

	if(counter.size() != 6) {
		std::cerr << "Encountered " << counter.size() << " characters instead of 6:";
		for(char c : chars) {
			std::cerr << c;
		}
		std::cerr << std::endl;
		return -1;
	}

	bool invalid = false;
	for(int c : counts) {
		if(c != kCubeWidth * kCubeWidth) {
			std::cerr << "Incorrect counts of sticker colors:" << std::endl;
			for(int i = 0; i < 6; ++i) {
				std::cerr << chars[i] << " : " << counts[i] << std::endl;
			}
			return -1;
		}
	}


	std::map<char, int> tempIndex;
	for(int i = 0; i < 6; ++i)
		tempIndex[chars[i]] = i;

	// map characters to faces
	bool opposites[6][6];
	for(int i = 0; i < 6; ++i)
		for(int j = 0; j < 6; ++j)
			opposites[i][j] = true;
	for(int i = 0; i < 6; ++i)
		opposites[i][i] = false;

	std::vector<int> corners{	data[0][0][0], data[2][kCubeWidth-1][0], data[4][0][kCubeWidth-1], 										// front top left
								data[0][0][kCubeWidth-1], data[2][kCubeWidth-1][kCubeWidth-1], data[1][0][0],							// front top right
								data[0][kCubeWidth-1][0], data[3][0][0], data[4][kCubeWidth-1][kCubeWidth-1], 							// front bottom left
								data[0][kCubeWidth-1][kCubeWidth-1], data[1][kCubeWidth-1][0], data[3][0][kCubeWidth-1],				// front bottom right
								data[2][0][0], data[4][0][0], data[5][0][kCubeWidth-1],													// back top left
								data[2][0][kCubeWidth-1], data[1][0][kCubeWidth-1], data[5][0][0],										// back top right
								data[3][kCubeWidth-1][0], data[4][kCubeWidth-1][0], data[5][kCubeWidth-1][kCubeWidth-1],				// back bottom left
								data[3][kCubeWidth-1][kCubeWidth-1], data[1][kCubeWidth-1][kCubeWidth-1], data[5][kCubeWidth-1][0]};	// back bottom right

	// for each corner
	for(unsigned int i = 0; i < corners.size(); i += 3) {
		int c1 = tempIndex[corners[i]];
		int c2 = tempIndex[corners[i+1]];
		int c3 = tempIndex[corners[i+2]];

		// std::cout << "C1,c2,c3 = " << c1 << " " << c2 << " " << c3 << std::endl; 

		opposites[c1][c2] = false;
		opposites[c1][c3] = false;
		opposites[c2][c1] = false;
		opposites[c2][c3] = false;
		opposites[c3][c1] = false;
		opposites[c3][c2] = false;
	}

	
	std::cout << "chars = ";
	for(char c : chars) {
		std::cout << c;
	}
	std::cout << std::endl;

	std::cout << "opposites:" << std::endl;
	for(int i = 0; i < 6; ++i) {
		for(int j = 0; j < 6; ++j) {
			std::cout << opposites[i][j];
		}
		std::cout << std::endl;
	}
	

	int count = 0;
	for(int i = 0; i < 6; ++i) {
		bool found = false;
		for(int j = 0; j < 6; ++j) {
			if(opposites[i][j]) {
				found = true;
				count++;
				break;
			} 
		}

		if(!found || count != (i+1)) {
			std::cerr << "Corners of cube are not solvable" << std::endl;
			return -1;
		}
	}

	int perm[6] = {-1, -1, -1, -1, -1, -1};

	for(int i = 0; i < 6; ++i) {
		if(perm[i] != -1) {
			continue;
		}

		perm[i] = i;
		for(int j = 0; j < 6; ++j) {
			if(opposites[i][j]) {
				perm[j] = 5-i;
				break;
			}
		}
	}

	for(int i = 0; i < 6; ++i) {
		for(int j = 0; j < kCubeWidth; ++j) {
			for(int k = 0; k < kCubeWidth; ++k) {
				char orig = data[i][j][k];
				int tempI = tempIndex[orig];
				solver->set(i, j, k, perm[tempI]);
			}
		}
	}

	colors.clear();
	colors.push_back(glm::uvec3(46, 139, 87));		// set default colors
	colors.push_back(glm::uvec3(255, 0, 0));
	colors.push_back(glm::uvec3(255, 255, 255));
	colors.push_back(glm::uvec3(255, 255, 0));
	colors.push_back(glm::uvec3(255, 140, 0));
	colors.push_back(glm::uvec3(0, 0, 255));

	if(obj.find("colors") != obj.end()) {
		json color_data = obj["colors"];

		for(int i = 0; i < 6; ++i) {
			
			char c = chars[i];
			std::string g(1, c);				// turn character into string
			if(color_data.find(g) == color_data.end()) {
				std::cerr << "Could not find symbol " << c << " in colors" << std::endl;
				return -1;
			}

			json single_rgb = color_data[g];	// get json list of 3 ints: rgb

			std::vector<int> temp;
			for(int i : single_rgb) {
				temp.push_back(i);
			}

			glm::uvec3 rgb(temp[0], temp[1], temp[2]);
			int pos = perm[tempIndex[c]];
			colors[pos] = rgb;					// put rgb as uvec3 in map
		}
	}

	std::cout << "JSON valid" << std::endl;
	return 0;
}