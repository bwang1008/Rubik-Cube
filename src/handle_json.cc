#include "handle_json.h"

#include <fstream>				// read in json file
#include <iostream>				// std::cerr, std::endl
#include <map>					// map from characters in json to int (index)

#include "config.h"				// compare constants N and kCubeWidth
#include "json.hpp"				// json handling

// used tutorial at http://rachelnertia.github.io/programming/2016/07/03/json-hpp/
int read_json(std::string& json_path, Solver* solver, std::vector<glm::uvec3>& colors) {
	std::cout << "hey!" << std::endl;
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

	// map characters to faces
	bool opposites[6][6];
	for(int i = 0; i < 6; ++i)
		for(int j = 0; j < 6; ++j)
			opposites[i][j] = true;
	for(int i = 0; i < 6; ++i)
		opposites[i][i] = false;

	



	std::map<char, int> map1;
	if(obj.find("colors") != obj.end()) {
		
	}

	std::cout << "JSON valid" << std::endl;
	return 0;
}