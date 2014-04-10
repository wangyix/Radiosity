#version 400


in int teQuadId;

//out int fColor;

void main(void) {
	
	//fColor = teQuadId;

	// TEST!!!!
	switch (teQuadId % 7) {
	case 0:
		gl_FragColor = vec4(1,0,0,1);
		break;
	case 1:
		gl_FragColor = vec4(0,1,0,1);
		break;
	case 2:
		gl_FragColor = vec4(0,0,1,1);
		break;
	case 3:
		gl_FragColor = vec4(1,1,0,1);
		break;
	case 4:
		gl_FragColor = vec4(1,0,1,1);
		break;
	case 5:
		gl_FragColor = vec4(0,1,1,1);
		break;
	default:
		gl_FragColor = vec4(1,1,1,1);
	}
}
