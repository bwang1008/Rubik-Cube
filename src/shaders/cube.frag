R"zzz(#version 330 core

flat in int type;
out vec4 fragment_color;
void main() {
	fragment_color = vec4(0.43, 0.73, 0.11, 1.0);

	if(type == 1) {
		fragment_color = vec4(0.8, 0.3, 0.4, 1.0);
	}
	if(type == 2) {
		fragment_color = vec4(0.0, 0.0, 1.0, 0.5);
	}
	if(type == 3) {
		fragment_color = vec4(0.6, 0.2, 0.3, 0.8);
	}
})zzz"
