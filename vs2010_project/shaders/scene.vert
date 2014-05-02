#version 400

// constants
uniform mat4 _modelViewProj;

// vertex shader input
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;

// vertex shader output
out vec2 vTexcoord;


void main(void) {

	gl_Position = _modelViewProj * vec4(position, 1.0);

	vTexcoord = texcoord;
}