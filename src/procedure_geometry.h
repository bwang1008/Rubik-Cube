#ifndef PROCEDURE_GEOMETRY_H
#define PROCEDURE_GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>
#include "cube.h"

void create_skybox(std::vector<glm::vec4>& sky_vertices, std::vector<glm::uvec3>& sky_faces, const glm::vec3 eye);
void create_rubik(std::vector<Cube*>& cubes, std::vector<glm::vec4>& cube_vertices, 
				std::vector<glm::uvec3>& cube_faces, std::vector<int>& cube_types, 
				std::vector<glm::vec3>& cube_centers);

void update_rubik2(std::vector<glm::vec3>& cube_centers, glm::vec3 move, std::vector<int>& cube_rotating);
void create_black_triangles(std::vector<glm::vec4>& black_triangle_vertices, std::vector<glm::uvec3>& black_triangle_faces, glm::ivec3 currentMove);
#endif
