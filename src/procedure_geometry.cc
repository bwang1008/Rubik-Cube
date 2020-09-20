#include "procedure_geometry.h"
#include "config.h"

void create_large_cube(std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces) {
	int L = kCubeWidth;
	float H = L / 2.0f;

	// have cube of length L, centered at (0, 0, 0)
	// put in the 8 vertices
	cube_vertices.push_back(glm::vec4(-H + 0, -H + 0, -H + 0, 1));
	cube_vertices.push_back(glm::vec4(-H + L, -H + 0, -H + 0, 1));
	cube_vertices.push_back(glm::vec4(-H + 0, -H + L, -H + 0, 1));
	cube_vertices.push_back(glm::vec4(-H + L, -H + L, -H + 0, 1));
	cube_vertices.push_back(glm::vec4(-H + 0, -H + 0, -H + L, 1));
	cube_vertices.push_back(glm::vec4(-H + L, -H + 0, -H + L, 1));
	cube_vertices.push_back(glm::vec4(-H + 0, -H + L, -H + L, 1));
	cube_vertices.push_back(glm::vec4(-H + L, -H + L, -H + L, 1));

	// put in the 12 faces, oriented in the same direction
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
}