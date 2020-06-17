R"zzz(#version 330 core

flat in int type;
in vec3 coord;

out vec4 fragment_color;

void main() {

	float u = coord[0];
	float v = coord[1];

	// front, right, top axis
	int first12Bits = (type >> 6) & 4095;

	int frontAxisBits = (first12Bits >> 8) & 15;
	int rightAxisBits = (first12Bits >> 4) & 15;
	int topAxisBits = first12Bits & 15;

	ivec3 frontAxis = ivec3((frontAxisBits >> 2) & 1, (frontAxisBits >> 1) & 1, frontAxisBits & 1);
	ivec3 rightAxis = ivec3((rightAxisBits >> 2) & 1, (rightAxisBits >> 1) & 1, rightAxisBits & 1);
	ivec3 topAxis = ivec3((topAxisBits >> 2) & 1, (topAxisBits >> 1) & 1, topAxisBits & 1);

	if((frontAxisBits >> 3) == 1) {
		frontAxis = -frontAxis;
	}
	if((rightAxisBits >> 3) == 1) {
		rightAxis = -rightAxis;
	}
	if((topAxisBits >> 3) == 1) {
		topAxis = -topAxis;
	}

	int frontAxisNum = 9 * frontAxis[0] + 3 * frontAxis[1] + frontAxis[2];
	int rightAxisNum = 9 * rightAxis[0] + 3 * rightAxis[1] + rightAxis[2];
	int topAxisNum = 9 * topAxis[0] + 3 * topAxis[1] + topAxis[2];

	vec4 white = vec4(1.0, 1.0, 1.0, 1.0); // white
	vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0); // yellow
	vec4 red = vec4(1.0, 0.0, 0.0, 1.0); // red
	vec4 orange = vec4(255.0f/255.0, 140.0f/255.0, 0.0, 1.0); // orange
	vec4 green = vec4(34.0/256, 139.0/256, 34.0/256, 1.0); // green
	//46,139,87
	//vec4 green = vec4(46.0/256, 139.0/256, 87.0/256, 1.0);
	vec4 blue = vec4(0.0, 0.0, 1.0, 1.0); // blue

	int myAxis = 0;

	if(coord[1] == 1.0) { // absolute up
		v = coord[2];
		
		//int bit = (type >> 3) & 1;
		//fragment_color = bit * white;

		myAxis = 3;
		
	}
	else if(coord[1] == 0.0) { // absolute down
		v = coord[2];

		//int bit = (type >> 2) & 1;
		//fragment_color = bit * yellow;

		myAxis = -3;
	}
	else if(coord[0] == 1.0) { // absolute right in x direction
		u = coord[1];
		v = coord[2];

		//int bit = (type >> 4) & 1;
		//fragment_color = bit * red;

		myAxis = 9;
	}
	else if(coord[0] == 0.0) { // absolute left in x direction
		u = coord[1];
		v = coord[2];

		//int bit = (type >> 1) & 1;
		//fragment_color = bit * orange;

		myAxis = -9;
	}
	else if(coord[2] == 1.0) { // absolute front in z direction
		u = coord[0];
		v = coord[1];

		//int bit = (type >> 5) & 1;
		//fragment_color = bit * green;

		myAxis = 1;
	}
	else if(coord[2] == 0.0) { // absolute back in z direction
		u = coord[0];
		v = coord[1];

		//int bit = (type >> 0) & 1;
		//fragment_color = blue;

		myAxis = -1;
	}



	// actual coloring of fragment
	if(myAxis == frontAxisNum) { // this top fragment is actually cube's front
		int bit = (type >> 5) & 1; // get bit for cube's front
		fragment_color = bit * green; // cube's front is always green
	}
	else if(myAxis == -frontAxisNum) { // this top fragment is actually cube's back
		int bit = (type >> 0) & 1; // get bit for cube's back
		fragment_color = bit * blue; // cube's back is always blue
	}
	else if(myAxis == rightAxisNum) { // this top fragment is actually cube's right
		int bit = (type >> 4) & 1; // get bit for cube's right
		fragment_color = bit * red; // cube's right is always red
	}
	else if(myAxis == -rightAxisNum) { // this top fragment is actually cube's left
		int bit = (type >> 1) & 1; // get bit for cube's left
		fragment_color = bit * orange; // cube's left is always orange
	}
	else if(myAxis == topAxisNum) { // this top fragment is actually cube's top
		int bit = (type >> 3) & 1; // get bit for cube's top
		fragment_color = bit * white; // cube's top is always white
	}
	else if(myAxis == -topAxisNum) { // this top fragment is actually cube's bottom
		int bit = (type >> 2) & 1; // get bit for cube's bottom
		fragment_color = bit * yellow; // cube's bottom is always yellow
	}



	float dx = min(u, 1.0-u);
	float dy = min(v, 1.0-v);

	float EPS = 0.02;

	if(dx < EPS || dy < EPS) {
		fragment_color = vec4(0.0, 0.0, 0.0, 1.0);
	}

	fragment_color[3] = 1.0; // prevent transparencies

})zzz"
