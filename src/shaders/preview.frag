R"zzz(#version 330 core

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;

uniform int N;

flat in int index;
in vec2 coord;

out vec4 fragment_color;

void main() {
	vec3 texColor = vec3(0.0, 0.0, 1.0);

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

	// trying to get borders around each sticker...
	float u = coord[0];
	float v = coord[1];

	float EPS = 0.03;
	float r = 0.2;												// circle radius for rounded corners
	
	vec4 not_sticker_color = vec4(0.9, 0.9, 0.9, 1.0);

	float relativeErrorU = abs((u * N) - floor(u * N + 0.5));	// floor(x+0.5) = round(x) ; don't know if round is always supported though
	float relativeErrorV = abs((v * N) - floor(v * N + 0.5));

	if(relativeErrorU <= EPS || relativeErrorV <= EPS) {
		fragment_color = not_sticker_color;				// add borders to each sticker
	}
	else if(relativeErrorU < r/2 && relativeErrorV <= r - sqrt(r*r - (relativeErrorU - r)*(relativeErrorU - r))) {
		fragment_color = not_sticker_color;				// if relative coordinates below circle, color black
	}
}


)zzz"