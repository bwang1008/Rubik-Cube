R"zzz(#version 330 core

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;

flat in int index;
in vec2 coord;

out vec4 fragment_color;

void main() {
	vec3 texColor = vec3(0.0, 0.0, 0.0);

	if(index == 0) {
		texColor = texture(texture0, coord).xyz;
	}
	else if(index == 1) {
		texColor = texture(texture1, coord).xyz;
	}
	else if(index == 2) {
		texColor = texture(texture2, coord).xyz;
	}
	else if(index == 3) {
		texColor = texture(texture3, coord).xyz;
	}
	else if(index == 4) {
		texColor = texture(texture4, coord).xyz;
	}
	else if(index == 5) {
		texColor = texture(texture5, coord).xyz;
	}

	fragment_color = vec4(texColor.rgb, 1.0);

	float smallU = min(coord[0], 1 - coord[0]);
	float smallV = min(coord[1], 1 - coord[1]);

	float EPS = 0.002;
	if(smallU <= EPS || smallV <= EPS) {
		fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
	}
}


)zzz"