#version 400

// constants
uniform mat4 _modelView;	// camera at shooter


layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;

out vec3 vPosition;
out vec2 vTexcoord;


void main(void) {

	// transform to view space of shooter
	vPosition = ( _modelView * vec4(position, 1.0) ).xyz;

	vTexcoord = texcoord;

	// convert texcoord to NDC, [0,1]->[-1,1]
	gl_Position = vec4(texcoord*2.0-1.0, 0.0, 1.0);
}
