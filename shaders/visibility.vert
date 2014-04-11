#version 400

// constants
uniform mat4 _modelView;


layout(location=0) in vec3 position;
layout(location=1) in uint quadId;

out vec3 vPosition;
out uint vQuadId;
out float vTessLevel;


#define MAX_TESS_LEVEL 32.0f
#define MIN_TESS_LEVEL 1.0f
#define DIST_FOR_MIN_TESS 10.0f

void main(void) {

	// transform to view space
	vPosition = ( _modelView * vec4(position, 1.0) ).xyz;

	vQuadId = quadId;

	// calculate distance from camera
	float d = length(vPosition);
	// calculate tessellation level based on distance
	vTessLevel = clamp((DIST_FOR_MIN_TESS/d)*MIN_TESS_LEVEL,
			MIN_TESS_LEVEL, MAX_TESS_LEVEL);
}