R"zzz(#version 330 core

uniform sampler2D preview_texture;
//uniform sampler2D preview_texture1;
//uniform sampler2D preview_texture2;
//uniform sampler2D preview_texture3;
//uniform sampler2D preview_texture4;
//uniform sampler2D preview_texture5;

flat in int index;
in vec2 coord;

out vec4 fragment_color;

void main() {
	vec3 texColor = texture(preview_texture, coord).xyz;
	fragment_color = vec4(texColor.rgb, 1.0);

	float smallU = min(coord[0], 1 - coord[0]);
	float smallV = min(coord[1], 1 - coord[1]);

	float EPS = 0.002;
	if(smallU <= EPS || smallV <= EPS) {
		fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
	}
}


)zzz"