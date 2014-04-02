#version 400

// constants
uniform mat4 _modelViewProj;

// vertex shader input
attribute vec3 position;
attribute vec2 texcoord;

// vertex shader output
varying vec2 uv;


void main(void) {
	// compute position
	gl_Position = _modelViewProj * vec4(position, 1.0);

	uv = texcoord;
}