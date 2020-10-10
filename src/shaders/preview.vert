R"zzz(#version 330 core

uniform mat4 projection;
uniform mat4 view;

in vec4 vertex_position;
in int my_face;

flat out int index; // which face
out vec2 coord; 	// uv coordinates

void main() {
	gl_Position = projection * view * vertex_position;

	int pos = gl_VertexID % 4;
	coord = vec2(pos / 2, pos % 2);

	index = my_face;

	//index = gl_VertexID / 6;

	/*
	if(pos == 0) {
		coord = vec2(0, 1);
	}
	else if(pos == 1) {
		coord = vec2(1, 1);
	}
	else if(pos == 2) {
		coord = vec2(0, 0);
	}
	else if(pos == 3) {
		coord = vec2(1, 0);
	}*/
}

)zzz"