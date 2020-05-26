#ifndef CUBE_H
#define CUBE_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <limits>
#include <algorithm>
#include "config.h"
#include <glm/glm.hpp>

struct Cube {
	const float L = cubieWidth;

	glm::vec4 pos; 	// World coordinates of smaller corner
	int type; 		// Bitmask regarding which of the 6 faces are shown
	//glm::mat4 trans; // overall transformation matrix
	
	// type = 15 bits in total
	// First 12: 4 bits for axis of Front face, 4 bits for axis of Right face, 4 bits for axis of Top face 
	// 1 bit for sign, then 3 bits of vector of absolute orientation
	// Last 6: 111111_2 means, in an arbitrarily chosen order,
	// Front, Right, Top, Bottom, Left, Back
	// for which faces to show color (black if bit is 0) (relative to cube)

	Cube(float x, float y, float z):
		pos(x, y, z, 1),
		type(0)
	{
	}

	Cube(int material, float x, float y, float z):
		pos(x, y, z, 1),
		type(material)
	{
	}

	glm::vec4 getPos() {
		return pos;
	}

	/*
	void setTrans(glm::mat4 mat) {
		for(int i = 0; i < 4; ++i) {
			for(int j = 0; j < 4; ++j) {
				trans[i][j] = mat[i][j];
			}
		}
	}
	*/

	void create_cube(std::vector<glm::vec4>& cube_vertices, 
							 std::vector<glm::uvec3>& cube_faces,
							 std::vector<int>& cube_types
							 ) {
		size_t index = cube_vertices.size();

		glm::vec4 pos2 = pos;

		cube_vertices.push_back(glm::vec4(pos2[0],     pos2[1],     pos2[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos2[0] + L, pos2[1],     pos2[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos2[0]    , pos2[1] + L, pos2[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos2[0] + L, pos2[1] + L, pos2[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos2[0]    , pos2[1]    , pos2[2] + L, 1));
		cube_vertices.push_back(glm::vec4(pos2[0] + L, pos2[1]    , pos2[2] + L, 1));
		cube_vertices.push_back(glm::vec4(pos2[0]    , pos2[1] + L, pos2[2] + L, 1));
		cube_vertices.push_back(glm::vec4(pos2[0] + L, pos2[1] + L, pos2[2] + L, 1));

		cube_faces.push_back(glm::uvec3(index + 0, index + 2, index + 1));
		cube_faces.push_back(glm::uvec3(index + 1, index + 2, index + 3));
		cube_faces.push_back(glm::uvec3(index + 1, index + 3, index + 5));
		cube_faces.push_back(glm::uvec3(index + 5, index + 3, index + 7));
		cube_faces.push_back(glm::uvec3(index + 0, index + 4, index + 2));
		cube_faces.push_back(glm::uvec3(index + 4, index + 6, index + 2));
		cube_faces.push_back(glm::uvec3(index + 2, index + 6, index + 3));
		cube_faces.push_back(glm::uvec3(index + 3, index + 6, index + 7));
		cube_faces.push_back(glm::uvec3(index + 0, index + 1, index + 4));
		cube_faces.push_back(glm::uvec3(index + 1, index + 5, index + 4));
		cube_faces.push_back(glm::uvec3(index + 4, index + 5, index + 7));
		cube_faces.push_back(glm::uvec3(index + 4, index + 7, index + 6));

		for(int i = 0; i < 8; i++)
			cube_types.push_back(type);
	}

};

#endif