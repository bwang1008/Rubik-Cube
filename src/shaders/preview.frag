R"zzz(#version 330 core

uniform sampler2D preview_texture0;
uniform sampler2D preview_texture1;
uniform sampler2D preview_texture2;
uniform sampler2D preview_texture3;
uniform sampler2D preview_texture4;
uniform sampler2D preview_texture5;

flat in int index;
in vec2 coord;

out vec4 fragment_color;

void main() {
	vec3 texColor = vec3(0.0, 0.0, 0.0);

	if(index == 0) {
		texColor = texture(preview_texture0, coord).xyz;
		//texColor = vec3(0.16, 0.5, 0.33);
	}
	else if(index == 1) {
		texColor = texture(preview_texture1, coord).xyz;
		//texColor = vec3(1.0, 0.0, 0.0);
	}
	else if(index == 2) {
		texColor = texture(preview_texture2, coord).xyz;
		//texColor = vec3(1.0, 1.0, 1.0);
	}
	else if(index == 3) {
		texColor = texture(preview_texture3, coord).xyz;
		//texColor = vec3(1.0, 1.0, 0.0);
	}
	else if(index == 4) {
		texColor = texture(preview_texture4, coord).xyz;
		//texColor = vec3(1.0, 0.5, 0.0);
	}
	else if(index == 5) {
		texColor = texture(preview_texture5, coord).xyz;
		//texColor = vec3(0.0, 0.0, 1.0);
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