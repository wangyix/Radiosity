#version 400


flat in uint teQuadId;

//layout(location=0) out vec4 fColor;	// FOR TESTING: renders quad IDs as colors
layout(location=0) out uvec4 fId;

void main(void) {
	
	fId = uvec4(teQuadId, 0, 0, 0);
	
	/*
	// FOR TESTING: renders quad IDs as colors
	switch (teQuadId % 7) {
	case 0:
		fColor = vec4(1,0,0,1);
		break;
	case 1:
		fColor = vec4(0,1,0,1);
		break;
	case 2:
		fColor = vec4(0,0,1,1);
		break;
	case 3:
		fColor = vec4(1,1,0,1);
		break;
	case 4:
		fColor = vec4(1,0,1,1);
		break;
	case 5:
		fColor = vec4(0,1,1,1);
		break;
	default:
		fColor = vec4(1,1,1,1);
	}
	*/
}
