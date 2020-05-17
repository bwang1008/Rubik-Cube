R"zzz(#version 330 core
uniform mat4 projection;
uniform mat4 view;

in vec4 vertex_position;

void main(){
	vec4 tmp = vertex_position;
	gl_Position = projection * view * tmp;
}

)zzz"
