#ifndef PROCEDURE_GEOMETRY_H
#define PROCEDURE_GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>
#include "cube.h"

void create_rubik(std::vector<glm::vec4>& cube_vertices, 
				std::vector<glm::uvec3>& cube_faces, std::vector<int>& cube_types, 
				std::vector<glm::vec3>& cube_centers);

void update_rubik(std::vector<glm::vec3>& cube_centers, glm::vec3 move, std::vector<int>& cube_rotating, bool firstTime);
void create_black_triangles(std::vector<glm::vec4>& black_triangle_vertices, std::vector<glm::uvec3>& black_triangle_faces, glm::ivec3 currentMove);
void create_rubik2(std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces);


//void turnPixels(Image* images[6]);

#endif
