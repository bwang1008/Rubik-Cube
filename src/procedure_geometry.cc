#include "procedure_geometry.h"
#include "config.h"

void create_rubik(std::vector<glm::vec4>& cube_vertices, 
		std::vector<glm::uvec3>& cube_faces, std::vector<int>& cube_types, 
		std::vector<glm::vec3>& cube_centers) {

	int N = cubeWidth;
	float half = N/2.0f;

	// front axis, right axis, top axis: <0,0,1>, <1, 0, 0>, <0, 1, 0> -> 0001 0100 0010 ******_2 
	// -> (256 + 64 + 2) * 64 = 322 * 64 = 20608
	int frontBits = 20608;

	// Special case when N = 1
	if(N == 1) {
		Cube c(frontBits + 63, -half, -half, -half);
		c.create_cube(cube_vertices, cube_faces, cube_types);
		for(int i = 0; i < 8; ++i)
			cube_centers.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		return;
	}

	std::vector<Cube> cubes;

	// faces
	for(int i = 1; i < N-1; ++i) {
		for(int j = 1; j < N-1; j++) {
			// types ensure inside of cube is black
			Cube c1(frontBits + 4, -half + i, -half + 0, -half + j); // Bottom
			Cube c2(frontBits + 8, -half + i, -half + N-1, - half + j); // Top
			Cube c3(frontBits + 2, -half + 0, -half + i, -half + j); // Left
			Cube c4(frontBits + 16,-half + N-1, -half + i, -half + j); // Right
			Cube c5(frontBits + 1, -half + i, -half + j, -half + 0); // Back
			Cube c6(frontBits + 32, -half + i, -half + j, -half + N-1); // Front

			c1.create_cube(cube_vertices, cube_faces, cube_types);
			c2.create_cube(cube_vertices, cube_faces, cube_types);
			c3.create_cube(cube_vertices, cube_faces, cube_types);
			c4.create_cube(cube_vertices, cube_faces, cube_types);
			c5.create_cube(cube_vertices, cube_faces, cube_types);
			c6.create_cube(cube_vertices, cube_faces, cube_types);

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
		Cube c1(frontBits + 5, -half + i, -half + 0, -half + 0);
		Cube c2(frontBits + 36, -half + i, -half + 0, -half + N-1);
		Cube c3(frontBits + 9, -half + i, -half + N-1, -half + 0);
		Cube c4(frontBits + 40, -half + i, -half + N-1, -half + N-1);

		Cube c5(frontBits + 3, -half + 0, -half + i, -half + 0);
		Cube c6(frontBits + 34, -half + 0, -half + i, -half + N-1);
		Cube c7(frontBits + 17, -half + N-1, -half + i, -half + 0);
		Cube c8(frontBits + 48, -half + N-1, -half + i, -half + N-1);

		Cube c9(frontBits + 6, -half + 0, -half + 0, -half + i);
		Cube c10(frontBits + 10, -half + 0, -half + N-1, -half + i);
		Cube c11(frontBits + 20, -half + N-1, -half + 0, -half + i);
		Cube c12(frontBits + 24, -half + N-1, -half + N-1, -half + i);

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

	std::vector<int> corner_types{7, 38, 11, 42, 21, 52, 25, 56};

	// corners
	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < 2; ++j) {
			for(int k = 0; k < 2; ++k) {
				Cube c(frontBits + corner_types[4*i+2*j+k], -half + i * (N-1), -half + j * (N-1), -half + k * (N-1));
				c.create_cube(cube_vertices, cube_faces, cube_types);
				cubes.push_back(c);
			}
		}
	}

	for(size_t i = 0; i < cubes.size(); ++i) {
		glm::vec3 pos = cubes[i].getPos();
		pos[0] += 0.5f;
		pos[1] += 0.5f;
		pos[2] += 0.5f;
		for(int j = 0; j < 8; ++j)
			cube_centers.push_back(pos);
	}
}

void update_rubik(std::vector<glm::vec3>& cube_centers, glm::vec3 move, std::vector<int>& cube_rotating, bool firstTime) {
	int N = cubeWidth;
	float half = N/2.0f;

	// Face 0 = Front, 1 = Right, 2 = Top, 3 = Bottom, 4 = Left, 5 = Back
	int face = int(move[0]);
	int layer = int(move[1]);

	if(face < 0) { // do nothing
		for(size_t i = 0; i < cube_rotating.size(); ++i)
			cube_rotating[i] = 0;
		return;
	}

	//cube_rotating.clear();
	if (firstTime) {
		for (size_t i = 0; i < cube_rotating.size(); ++i) {
			cube_rotating[i] = 0;
		}
	}


	// Build point P and Q, where unit vector PQ is axis rotating about
	glm::vec3 P = glm::vec3(0.0f, 0.0f, 0.0f);
	
	if(face == 0) { // Front
		P[2] = N - 0.5f - layer - half;
	}
	else if(face == 1) { // Right
		P[0] = N - 0.5f - layer - half;
	}
	else if(face == 2) { // Top
		P[1] = N - 0.5f - layer - half;
	}
	else if(face == 3) { // Bottom
		P[1] = 0.5f + layer - half;
	}
	else if(face == 4) { // Left
		P[0] = 0.5f + layer - half;
	}
	else if(face == 5) { // Back
		P[2] = 0.5f + layer - half;
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

	// Iterate across all cubes where vector (P, center-of-cube) DOT (PQ) is 0
	// Want cubes in plane containing P and perpendicular to PQ 
	for(size_t i = 0; i < cube_centers.size(); i += 8) {
		glm::vec3 cube_center = cube_centers[i];

		glm::vec3 PC = cube_center - P;

		float dot = glm::dot(PC, PQ);

		if(abs(dot) < 0.00001f) {
			for (int j = 0; j < 8; ++j) {
				// cube_rotating.push_back(1);
				cube_rotating[i + j] = 1;
			}
		}
		else {
			for (int j = 0; j < 8; ++j) {
				// cube_rotating.push_back(0);
				
			}
		}
	}
}

void create_black_triangles(std::vector<glm::vec4>& black_triangle_vertices, std::vector<glm::uvec3>& black_triangle_faces, glm::ivec3 currentMove) {
	int N = cubeWidth;
	float half = N / 2.0f;

	int face = currentMove[0];
	int layer = currentMove[1];


}

void create_rubik2(std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces) {
	float half = cubeWidth / 2.0f;

	// front
	cube_vertices.push_back(glm::vec4(-half, half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, -half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, -half, half, 1.0f));

	cube_faces.push_back(glm::uvec3(0, 1, 2));
	cube_faces.push_back(glm::uvec3(1, 3, 2));

	// right
	cube_vertices.push_back(glm::vec4(half, half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, -half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, -half, -half, 1.0f));

	cube_faces.push_back(glm::uvec3(4, 5, 6));
	cube_faces.push_back(glm::uvec3(5, 7, 6));

	// up
	cube_vertices.push_back(glm::vec4(-half, half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, half, half, 1.0f));

	cube_faces.push_back(glm::uvec3(8 + 0, 8 + 1, 8 + 2));
	cube_faces.push_back(glm::uvec3(8 + 1, 8 + 3, 8 + 2));

	// down
	cube_vertices.push_back(glm::vec4(-half, -half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, -half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, -half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, -half, -half, 1.0f));

	cube_faces.push_back(glm::uvec3(12 + 0, 12 + 1, 12 + 2));
	cube_faces.push_back(glm::uvec3(12 + 1, 12 + 3, 12 + 2));

	// left
	cube_vertices.push_back(glm::vec4(-half, half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, -half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, half, half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, -half, half, 1.0f));

	cube_faces.push_back(glm::uvec3(16 + 0, 16 + 1, 16 + 2));
	cube_faces.push_back(glm::uvec3(16 + 1, 16 + 3, 16 + 2));

	// back
	cube_vertices.push_back(glm::vec4(half, half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(half, -half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, half, -half, 1.0f));
	cube_vertices.push_back(glm::vec4(-half, -half, -half, 1.0f));

	cube_faces.push_back(glm::uvec3(20 + 0, 20 + 1, 20 + 2));
	cube_faces.push_back(glm::uvec3(20 + 1, 20 + 3, 20 + 2));
}