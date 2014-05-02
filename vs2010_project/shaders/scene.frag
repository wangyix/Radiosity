#version 400

// constants
uniform sampler2D _tex;
uniform int _allWhite;

// fragment shader input
in vec2 vTexcoord;


void main(void) {
	
	if (_allWhite) {
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	} else {
		gl_FragColor = vec4(texture2D(_tex, vTexcoord).xyz, 1.0);
	}
}
