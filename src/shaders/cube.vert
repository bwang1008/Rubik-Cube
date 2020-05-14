R"zzz(#version 330 core
uniform mat4 projection;
uniform mat4 view;

in vec4 vertex_position;
in int cube_type;
flat out int type;

void main(){
	vec4 tmp = vertex_position;
	gl_Position = projection * view * tmp;
	type = cube_type;
}

)zzz"
