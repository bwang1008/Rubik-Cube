R"zzz(#version 330 core
uniform mat4 projection;
uniform mat4 view;

in vec4 vertex_position;
in int cube_type;
in vec4 col0;
in vec4 col1;
in vec4 col2;
in vec4 col3;

flat out int type;
out vec3 coord;

void main(){
	mat4 mat = mat4(col0, col1, col2, col3);

	vec4 tmp = vertex_position;
	gl_Position = projection * view * mat * tmp;
	type = cube_type;
	int pos = gl_VertexID % 8;
	coord = vec3(pos % 2, pos/2 % 2, pos/4 % 2);
}

)zzz"
