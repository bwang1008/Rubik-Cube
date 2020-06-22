R"zzz(#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform int face;
uniform float theta;

in vec4 vertex_position;
in int cube_type;
in int rotating;

flat out int type;
out vec3 coord;

void main(){
	
	mat4 mat = mat4(1.0f);
	if(rotating > 0) {

		if(face == 0) {
			mat[0][0] = cos(theta);
			mat[1][0] = -sin(theta);
			mat[0][1] = sin(theta);
			mat[1][1] = cos(theta);
			
		}
		else if(face == 1) { // right
			mat[1][1] = cos(theta);
			mat[2][1] = -sin(theta);
			mat[1][2] = sin(theta);
			mat[2][2] = cos(theta);
		}
		else if(face == 2) {
			mat[0][0] = cos(theta);
			mat[2][0] = sin(theta);
			mat[0][2] = -sin(theta);
			mat[2][2] = cos(theta);
		}
		else if(face == 3) {
			mat[0][0] = cos(theta);
			mat[2][0] = -sin(theta);
			mat[0][2] = sin(theta);
			mat[2][2] = cos(theta);
		}
		else if(face == 4) {
			mat[1][1] = cos(theta);
			mat[2][1] = sin(theta);
			mat[1][2] = -sin(theta);
			mat[2][2] = cos(theta);
		}
		else if(face == 5) {
			mat[0][0] = cos(theta);
			mat[1][0] = sin(theta);
			mat[0][1] = -sin(theta);
			mat[1][1] = cos(theta);
		}
	}

	vec4 tmp = vertex_position;
	gl_Position = projection * view * mat * tmp;
	type = cube_type;
	int pos = gl_VertexID % 8;
	coord = vec3(pos % 2, pos/2 % 2, pos/4 % 2);
}

)zzz"
