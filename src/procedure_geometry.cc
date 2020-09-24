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
	int index = 0;
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

/*
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
*/