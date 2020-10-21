#include "procedure_geometry.h"
#include "config.h"

#include <iostream>

/**
*	Fills cube_vertices with 24 vertices (each of the 8 vertices are used in 3 different faces)
*   Fills cube_faces with 12 triangles (2 triangles for each of the 6 faces)
*/
void create_large_cube(std::vector<glm::vec4>& cube_vertices, std::vector<glm::uvec3>& cube_faces) {
	float half = kCubeWidth / 2.0f;

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

/**
*	All sticker colors are currently stored in solver parameter
*	Need to fill images parameter with the correct rgb values
* 	images parameter are used to texture the big cube
*/
void create_textures(Image* images[6], Solver* solver, std::vector<glm::uvec3> rgbs) {
	// assumes solver.faces is already filled
	
	int append = (4 - ((3 * kCubeWidth) % 4)) % 4;	// Weird alignment with Image: needs number of bytes on width, to be divisible by 4.

	for(int f = 0; f < 6; ++f) {					// for each of the six faces,
		Image* im = new Image();					// create a base texture / image

		im->width  = kCubeWidth;					// fill in width and height
		im->height = kCubeWidth;

		for(int i = 0; i < kCubeWidth; ++i) {
			for(int j = 0; j < kCubeWidth; ++j) {	// fill in each row and column of texture / image
				int color = solver->get(f, i, j);	// grab the value given from Solver.faces[6][N][N]
				im->bytes.push_back(rgbs[color][0]);	
				im->bytes.push_back(rgbs[color][1]);
				im->bytes.push_back(rgbs[color][2]);	// append each R,G,B value of the color
			}
			for(int j = 0; j < append; ++j) {		// append padding so alignment divisible by 4
				im->bytes.push_back(0);
			}
		}

		images[f] = im;								// Assign created Image into parameter images
	}
}
