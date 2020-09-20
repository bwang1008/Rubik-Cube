#ifndef HANDLE_JSON_H
#define HANDLE_JSON_H

#include "json.hpp"
#include "solver.h"

#include <glm/glm.hpp>
#include <string>

int read_json(std::string& json_path, Solver* solver, std::vector<glm::uvec3>& colors);

#endif