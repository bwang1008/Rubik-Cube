#include "procedure_geometry.h"
#include "config.h"

void create_skybox(std::vector<glm::vec4>& sky_vertices, std::vector<glm::uvec3>& sky_faces, const glm::vec3 eye)
{
	int skyLimit = 200;
	int L = skyLimit;

	int x = eye[0];
	int y = eye[1];
	int z = eye[2];

	int index = 0;

	sky_vertices.push_back(glm::vec4(eye[0] - L, eye[1] - L, eye[2] - L, 1));
	sky_vertices.push_back(glm::vec4(eye[0] + L, eye[1] - L, eye[2] - L, 1));
	sky_vertices.push_back(glm::vec4(eye[0] - L, eye[1] + L, eye[2] - L, 1));
	sky_vertices.push_back(glm::vec4(eye[0] + L, eye[1] + L, eye[2] - L, 1));
	sky_vertices.push_back(glm::vec4(eye[0] - L, eye[1] - L, eye[2] + L, 1));
	sky_vertices.push_back(glm::vec4(eye[0] + L, eye[1] - L, eye[2] + L, 1));
	sky_vertices.push_back(glm::vec4(eye[0] - L, eye[1] + L, eye[2] + L, 1));
	sky_vertices.push_back(glm::vec4(eye[0] + L, eye[1] + L, eye[2] + L, 1));

	sky_faces.push_back(glm::uvec3(index + 0, index + 1, index + 2));
	sky_faces.push_back(glm::uvec3(index + 1, index + 3, index + 2));
	sky_faces.push_back(glm::uvec3(index + 1, index + 5, index + 3));
	sky_faces.push_back(glm::uvec3(index + 5, index + 7, index + 3));
	sky_faces.push_back(glm::uvec3(index + 0, index + 2, index + 4));
	sky_faces.push_back(glm::uvec3(index + 4, index + 2, index + 6));
	sky_faces.push_back(glm::uvec3(index + 2, index + 3, index + 6));
	sky_faces.push_back(glm::uvec3(index + 3, index + 7, index + 6));
	sky_faces.push_back(glm::uvec3(index + 0, index + 4, index + 1));
	sky_faces.push_back(glm::uvec3(index + 1, index + 4, index + 5));
	sky_faces.push_back(glm::uvec3(index + 4, index + 7, index + 5));
	sky_faces.push_back(glm::uvec3(index + 4, index + 6, index + 7));
}

