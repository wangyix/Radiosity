#version 400

// constants
uniform mat4 _modelView;


layout(location=0) in vec3 position;
layout(location=1) in uint quadId;

out vec3 vPosition;
out uint vQuadId;



void main(void) {

	// transform to view space
	vPosition = ( _modelView * vec4(position, 1.0) ).xyz;

	vQuadId = quadId;
}