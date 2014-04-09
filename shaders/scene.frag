#version 400

// constants
uniform sampler2D _tex;

// fragment shader input
in vec2 vTexcoord;


void main(void) {
	
	gl_FragColor = vec4(texture2D(_tex, vTexcoord).xyz, 1.0);

}
