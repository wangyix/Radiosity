#version 400

// constants
uniform sampler2D _tex;

// fragment shader input
varying vec2 uv;


void main(void) {
	gl_FragColor = vec4(texture2D(_tex, uv).xyz, 1.0);
}