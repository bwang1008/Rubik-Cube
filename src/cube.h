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
	int L = cubieWidth;

	glm::vec4 pos; 	// World coordinates of smaller corner
	int type; 		// Dirt, grass, water,...
	Cube(int x, int y, int z):
		pos(x, y, z, 1),
		type(0)
	{
	}

	Cube(int material, int x, int y, int z):
		pos(x, y, z, 1),
		type(material)
	{
	}

	void create_cube(std::vector<glm::vec4>& cube_vertices, 
							 std::vector<glm::uvec3>& cube_faces,
							 std::vector<int>& cube_types
							 ) {
		size_t index = cube_vertices.size();

		cube_vertices.push_back(glm::vec4(pos[0],     pos[1],     pos[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos[0] + L, pos[1],     pos[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos[0]    , pos[1] + L, pos[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos[0] + L, pos[1] + L, pos[2]    , 1));
		cube_vertices.push_back(glm::vec4(pos[0]    , pos[1]    , pos[2] + L, 1));
		cube_vertices.push_back(glm::vec4(pos[0] + L, pos[1]    , pos[2] + L, 1));
		cube_vertices.push_back(glm::vec4(pos[0]    , pos[1] + L, pos[2] + L, 1));
		cube_vertices.push_back(glm::vec4(pos[0] + L, pos[1] + L, pos[2] + L, 1));

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