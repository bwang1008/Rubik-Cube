R"zzz(#version 330 core
uniform mat4 projection;
uniform mat4 view;

in vec4 vertex_position;
in int cube_type;

flat out int type;
out vec3 coord;

void main(){
	vec4 tmp = vertex_position;
	gl_Position = projection * view * tmp;
	type = cube_type;
	int pos = gl_VertexID % 8;
	coord = vec3(pos % 2, pos/2 % 2, pos/4 % 2);
}

)zzz"
