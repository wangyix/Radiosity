#version 400


layout(location=0) in vec2 texcoord;

out vec2 vTexcoord;


void main(void) {

	vTexcoord = texcoord;

	// convert texcoord to NDC, [0,1]->[-1,1]
	gl_Position = vec4(texcoord*2.0-1.0, 0.0, 1.0);
}