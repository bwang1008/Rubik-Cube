R"zzz(#version 330 core

flat in int type;
in vec3 coord;

out vec4 fragment_color;

void main() {

	float u = coord[0];
	float v = coord[1];

	if(coord[1] == 1.0) { // top face
		v = coord[2];
		int bit = (type >> 3) & 1;

		fragment_color = bit * vec4(1.0, 1.0, 1.0, 1.0); // white
	}
	else if(coord[1] == 0.0) { // bottom face
		v = coord[2];

		int bit = (type >> 2) & 1;
		fragment_color = bit * vec4(1.0, 1.0, 0.0, 1.0); // yellow
	}
	else if(coord[0] == 1.0) { // right
		u = coord[1];
		v = coord[2];

		int bit = (type >> 4) & 1;

		fragment_color = bit * vec4(1.0, 0.0, 0.0, 1.0); // red
	}
	else if(coord[0] == 0.0) { // left
		u = coord[1];
		v = coord[2];

		int bit = (type >> 1) & 1;

		fragment_color = bit * vec4(255.0f/255.0, 100.0f/255.0, 0.0, 1.0); // orange
	}
	else if(coord[2] == 1.0) { // front
		u = coord[0];
		v = coord[1];

		int bit = (type >> 5) & 1;

		fragment_color = bit * vec4(34.0/256, 139.0/256, 34.0/256, 1.0); // green
	}
	else if(coord[2] == 0.0) { // back
		u = coord[0];
		v = coord[1];

		int bit = (type >> 0) & 1;

		fragment_color = bit * vec4(0.0, 0.0, 1.0, 1.0); // blue
	}

	float dx = min(u, 1.0-u);
	float dy = min(v, 1.0-v);

	float EPS = 0.02;

	if(dx < EPS || dy < EPS) {
		fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
	}

})zzz"
