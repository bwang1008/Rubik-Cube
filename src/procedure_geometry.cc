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

void create_rubik(std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces, std::vector<int>& cube_types) {
	int N = cubeWidth;

	// faces
	for(int i = 1; i < N-1; ++i) {
		for(int j = 1; j < N-1; j++) {
			Cube c1 = Cube(1, i, 0, j); // front
			Cube c2 = Cube(1, i, N-1, j); // back
			Cube c3 = Cube(1, 0, i, j);
			Cube c4 = Cube(1, N-1, i, j);
			Cube c5 = Cube(1, i, j, 0);
			Cube c6 = Cube(1, i, j, N-1);

			c1.create_cube(cube_vertices, cube_faces, cube_types);
			c2.create_cube(cube_vertices, cube_faces, cube_types);
			c3.create_cube(cube_vertices, cube_faces, cube_types);
			c4.create_cube(cube_vertices, cube_faces, cube_types);
			c5.create_cube(cube_vertices, cube_faces, cube_types);
			c6.create_cube(cube_vertices, cube_faces, cube_types);
		}
	}

	// edges
	for(int i = 1; i < N-1; ++i) {
		Cube c1 = Cube(1, i, 0, 0);
		Cube c2 = Cube(1, i, 0, N-1);
		Cube c3 = Cube(1, i, N-1, 0);
		Cube c4 = Cube(1, i, N-1, N-1);

		Cube c5 = Cube(1, 0, i, 0);
		Cube c6 = Cube(1, 0, i, N-1);
		Cube c7 = Cube(1, N-1, i, 0);
		Cube c8 = Cube(1, N-1, i, N-1);

		Cube c9 = Cube(1, 0, 0, i);
		Cube c10 = Cube(1, 0, N-1, i);
		Cube c11 = Cube(1, N-1, 0, i);
		Cube c12 = Cube(1, N-1, N-1, i);

		c1.create_cube(cube_vertices, cube_faces, cube_types);
		c2.create_cube(cube_vertices, cube_faces, cube_types);
		c3.create_cube(cube_vertices, cube_faces, cube_types);
		c4.create_cube(cube_vertices, cube_faces, cube_types);
		c5.create_cube(cube_vertices, cube_faces, cube_types);
		c6.create_cube(cube_vertices, cube_faces, cube_types);
		c7.create_cube(cube_vertices, cube_faces, cube_types);
		c8.create_cube(cube_vertices, cube_faces, cube_types);
		c9.create_cube(cube_vertices, cube_faces, cube_types);
		c10.create_cube(cube_vertices, cube_faces, cube_types);
		c11.create_cube(cube_vertices, cube_faces, cube_types);
		c12.create_cube(cube_vertices, cube_faces, cube_types);
	}

	// corners
	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < 2; ++j) {
			for(int k = 0; k < 2; ++k) {
				Cube c = Cube(1, i * (N-1), j * (N-1), k * (N-1));
				c.create_cube(cube_vertices, cube_faces, cube_types);
			}
		}
	}

}