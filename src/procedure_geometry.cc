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

void create_rubik(std::vector<Cube*>& cubes, std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces, std::vector<int>& cube_types, std::vector<glm::mat4>& trans) {
	int N = cubeWidth;
	float half = N/2.0f;

	// faces
	for(int i = 1; i < N-1; ++i) {
		for(int j = 1; j < N-1; j++) {
			// types ensure inside of cube is black
			Cube* c1 = new Cube(4, -half + i, -half + 0, -half + j); // Top
			Cube* c2 = new Cube(8, -half + i, -half + N-1, - half + j); // Bottom
			Cube* c3 = new Cube(2, -half + 0, -half + i, -half + j); // Left
			Cube* c4 = new Cube(16,-half + N-1, -half + i, -half + j); // Right
			Cube* c5 = new Cube(1, -half + i, -half + j, -half + 0); // Front
			Cube* c6 = new Cube(32, -half + i, -half + j, -half + N-1); // Back

			c1 -> create_cube(cube_vertices, cube_faces, cube_types);
			c2 -> create_cube(cube_vertices, cube_faces, cube_types);
			c3 -> create_cube(cube_vertices, cube_faces, cube_types);
			c4 -> create_cube(cube_vertices, cube_faces, cube_types);
			c5 -> create_cube(cube_vertices, cube_faces, cube_types);
			c6 -> create_cube(cube_vertices, cube_faces, cube_types);

			cubes.push_back(c1);
			cubes.push_back(c2);
			cubes.push_back(c3);
			cubes.push_back(c4);
			cubes.push_back(c5);
			cubes.push_back(c6);
		}
	}

	// edges
	for(int i = 1; i < N-1; ++i) {
		Cube* c1 = new Cube(63, -half + i, -half + 0, -half + 0);
		Cube* c2 = new Cube(63, -half + i, -half + 0, -half + N-1);
		Cube* c3 = new Cube(63, -half + i, -half + N-1, -half + 0);
		Cube* c4 = new Cube(63, -half + i, -half + N-1, -half + N-1);

		Cube* c5 = new Cube(63, -half + 0, -half + i, -half + 0);
		Cube* c6 = new Cube(63, -half + 0, -half + i, -half + N-1);
		Cube* c7 = new Cube(63, -half + N-1, -half + i, -half + 0);
		Cube* c8 = new Cube(63, -half + N-1, -half + i, -half + N-1);

		Cube* c9 = new Cube(63, -half + 0, -half + 0, -half + i);
		Cube* c10 = new Cube(63, -half + 0, -half + N-1, -half + i);
		Cube* c11 = new Cube(63, -half + N-1, -half + 0, -half + i);
		Cube* c12 = new Cube(63, -half + N-1, -half + N-1, -half + i);

		c1 -> create_cube(cube_vertices, cube_faces, cube_types);
		c2 -> create_cube(cube_vertices, cube_faces, cube_types);
		c3 -> create_cube(cube_vertices, cube_faces, cube_types);
		c4 -> create_cube(cube_vertices, cube_faces, cube_types);
		c5 -> create_cube(cube_vertices, cube_faces, cube_types);
		c6 -> create_cube(cube_vertices, cube_faces, cube_types);
		c7 -> create_cube(cube_vertices, cube_faces, cube_types);
		c8 -> create_cube(cube_vertices, cube_faces, cube_types);
		c9 -> create_cube(cube_vertices, cube_faces, cube_types);
		c10 -> create_cube(cube_vertices, cube_faces, cube_types);
		c11 -> create_cube(cube_vertices, cube_faces, cube_types);
		c12 -> create_cube(cube_vertices, cube_faces, cube_types);

		cubes.push_back(c1);
		cubes.push_back(c2);
		cubes.push_back(c3);
		cubes.push_back(c4);
		cubes.push_back(c5);
		cubes.push_back(c6);
		cubes.push_back(c7);
		cubes.push_back(c8);
		cubes.push_back(c9);
		cubes.push_back(c10);
		cubes.push_back(c11);
		cubes.push_back(c12);
	}

	// corners
	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < 2; ++j) {
			for(int k = 0; k < 2; ++k) {
				Cube* c = new Cube(63, -half + i * (N-1), -half + j * (N-1), -half + k * (N-1));
				c -> create_cube(cube_vertices, cube_faces, cube_types);
				cubes.push_back(c);
			}
		}
	}

	// originally, all cubes are not transformed (identity matrices)
	for(size_t i = 0; i < 8 * cubes.size(); i++) {
		glm::mat4 id = glm::mat4(1.0f);
		trans.push_back(id);
	}


}

void update_rubik(std::vector<Cube*>& cubes, std::vector<glm::mat4>& trans, glm::vec3 move, float time) {
	int N = cubeWidth;
	float half = N/2.0f;

	// Build point P and Q, where unit vector PQ is axis rotating about
	glm::vec3 P = glm::vec3(0.0f, 0.0f, 0.0f);


	// Face 0 = Front, 1 = Right, 2 = Top, 3 = Bottom, 4 = Left, 5 = Back
	int face = move[0];
	int layer = move[1];
	
	if(face == 0) { // Front
		P[2] = N - 0.5 - layer - half;
	}
	else if(face == 1) { // Right
		P[0] = N - 0.5 - layer - half;
	}
	else if(face == 2) { // Top
		P[1] = N - 0.5 - layer - half;
	}
	else if(face == 3) { // Bottom
		P[1] = 0.5 + layer - half;
	}
	else if(face == 4) { // Left
		P[0] = 0.5 + layer - half;
	}
	else if(face == 5) { // Back
		
		P[2] = 0.5 + layer - half;
	}

	glm::vec3 Q = glm::vec3(P[0], P[1], P[2]);

	if(face == 0) { // Front
		Q[2] -= 1.0f;
	}
	else if(face == 1) { // Right
		Q[0] += 1.0f;
	}
	else if(face == 2) { // Top
		Q[1] += 1.0f;
	}
	else if(face == 3) { // Bottom
		Q[1] -= 1.0f;
	}
	else if(face == 4) { // Left
		Q[0] -= 1.0f;
	}
	else if(face == 5) { // Back
		Q[2] += 1.0f;
	}

	// Build vector PQ
	glm::vec3 PQ = Q - P;

	// Build appropriate rotation matrix
	float theta = -0.03;
	glm::mat4 mat = glm::mat4(1.0f);

	if(face == 0) {
		mat[0][0] = cos(theta);
		mat[1][0] = -sin(theta);
		mat[0][1] = sin(theta);
		mat[1][1] = cos(theta);
	}
	else if(face == 1) { // right
		mat[1][1] = cos(theta);
		mat[2][1] = -sin(theta);
		mat[1][2] = sin(theta);
		mat[2][2] = cos(theta);
	}
	else if(face == 2) {
		mat[0][0] = cos(theta);
		mat[2][0] = sin(theta);
		mat[0][2] = -sin(theta);
		mat[2][2] = cos(theta);
	}
	else if(face == 3) {
		mat[0][0] = cos(theta);
		mat[2][0] = -sin(theta);
		mat[0][2] = sin(theta);
		mat[2][2] = cos(theta);
	}
	else if(face == 4) {
		mat[1][1] = cos(theta);
		mat[2][1] = sin(theta);
		mat[1][2] = -sin(theta);
		mat[2][2] = cos(theta);
	}
	else if(face == 5) {
		mat[0][0] = cos(theta);
		mat[1][0] = sin(theta);
		mat[0][1] = -sin(theta);
		mat[1][1] = cos(theta);
	}
	


	// Iterate across all cubes where vector (P, center-of-cube) DOT (PQ) is 0
	// Want cubes in plane containing P and perpendicular to PQ 
	
	int count = 0;

	for(size_t i = 0; i < cubes.size(); ++i) {
		Cube* c = cubes[i];
		glm::vec3 cube_center = glm::vec3(c -> getPos());
		cube_center[0] += 0.5;
		cube_center[1] += 0.5;
		cube_center[2] += 0.5;

		glm::vec3 PC = cube_center - P;

		float dot = glm::dot(PC, PQ);

		if(abs(dot) < 0.00001f) {
			//std::cout << "count = " << count << std::endl;
			count++;

			for(int j = 0; j < 8; ++j)
				trans[8*i + j] *= mat;
		}
	}
	//std::cout << "count = " << count << std::endl;
}