#version 400

// constants
uniform mat4 _mv;			// modelview
uniform mat3 _mvInvTrans;	// modelview inverse transpose
uniform mat4 _mvProj;		// modelviewproj

// vertex shader input
attribute vec3 position;
attribute vec3 normal;
attribute vec2 texcoord;

// vertex shader output
varying vec3 n;
varying vec2 uv;


void main(void) {
	// compute position
	gl_Position = _mvProj * vec4(position, 1.0);

	uv = texcoord;
	// compute light info
	n = normalize(_mvInvTrans * normal);
}