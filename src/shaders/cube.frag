R"zzz(#version 330 core

flat in int type;
in vec3 coord;

out vec4 fragment_color;

void main() {
	fragment_color = vec4(0.43, 0.73, 0.11, 1.0);

	if(type == 1) {
		fragment_color = vec4(1.0, 1.0, 1.0, 1.0);
	}
	if(type == 2) {
		fragment_color = vec4(0.0, 0.0, 1.0, 0.5);
	}
	if(type == 3) {
		fragment_color = vec4(0.6, 0.2, 0.3, 0.8);
	}

	float u = coord[0];
	float v = coord[1];

	if(coord[1] == 1.0) { // top face
		v = coord[2];
	}
	else if(coord[1] == 0.0) { // bottom face
		v = coord[2];
	}
	else if(coord[0] == 0.0 || coord[0] == 1.0) { 
		u = coord[1];
		v = coord[2];
	}
	else if(coord[2] == 0.0 || coord[2] == 1.0) {
		u = coord[0];
		v = coord[1];
	}

	float dx = min(u, 1.0-u);
	float dy = min(v, 1.0-v);

	float EPS = 0.02;

	if(dx < EPS || dy < EPS) {
		fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
	}

})zzz"
