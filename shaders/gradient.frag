#version 400

// constants
uniform vec2 _radTexelSize;
uniform float _threshold;

uniform sampler2D _radTex;


in vec2 vTexcoord;


void main(void) {

	vec2 right = vec2(_radTexelSize.s, 0.0);
	vec2 up = vec2(0.0, _radTexelSize.t);

	vec3 c = texture2D(_radTex, vTexcoord).xyz;

if (c.x >= 1.0)
	discard;

	vec3 l = texture2D(_radTex, vTexcoord-right).xyz;
	vec3 r = texture2D(_radTex, vTexcoord+right).xyz;
	vec3 b = texture2D(_radTex, vTexcoord-up).xyz;
	vec3 t = texture2D(_radTex, vTexcoord+up).xyz;
	
	// find horizontal gradient change
	float hChange = length((c-l)-(r-c));
	if (hChange > _threshold) {
		discard;
	}	

	// find vertical gradient change
	float vChange = length((c-b)-(t-c));
	if (vChange > _threshold) {
		discard;
	}

	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
