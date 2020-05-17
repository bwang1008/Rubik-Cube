#ifndef PROCEDURE_GEOMETRY_H
#define PROCEDURE_GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>
#include "cube.h"

void create_skybox(std::vector<glm::vec4>& sky_vertices, std::vector<glm::uvec3>& sky_faces, const glm::vec3 eye);
void create_rubik(std::vector<Cube*>& cubes, std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces, std::vector<int>& cube_types, std::vector<glm::mat4>& trans);
void update_rubik(std::vector<Cube*>& cubes, std::vector<glm::mat4>& trans, glm::vec3 move);

#endif
