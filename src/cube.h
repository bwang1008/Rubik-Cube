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
	glm::mat4 trans; // overall transformation matrix

	Cube(int x, int y, int z):
		pos(x, y, z, 1),
		type(0),
		trans(1.0f)
	{
	}

	Cube(int material, int x, int y, int z):
		pos(x, y, z, 1),
		type(material),
		trans(1.0f)
	{
	}

	void create_cube(std::vector<glm::vec4>& cube_vertices, 
							 std::vector<glm::uvec3>& cube_faces,
							 std::vector<int>& cube_types
							 ) {
		size_t index = cube_vertices.size();

		glm::vec4 pos2 = trans * pos;

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