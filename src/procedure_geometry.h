#ifndef PROCEDURE_GEOMETRY_H
#define PROCEDURE_GEOMETRY_H

#include <glm/glm.hpp>
#include <vector>

#include "solver.h"

void create_large_cube(std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces);
// void create_textures(Image* images[], Solver* solver, std::vector<glm::uvec3> rgbs);


#endif
