#version 400

// constants
uniform vec2 _radTexelSize;
uniform float _threshold;

uniform sampler2D _radTex;


in vec2 vTexcoord;


void main(void) {

	vec2 right = vec2(_radTexelSize.s, 0.0);
	vec2 up = vec2(0.0, _radTexelSize.t);

	vec3 c = clamp(texture2D(_radTex, vTexcoord).xyz, 0.0, 1.0);
	vec3 l = clamp(texture2D(_radTex, vTexcoord-right).xyz, 0.0, 1.0);
	vec3 r = clamp(texture2D(_radTex, vTexcoord+right).xyz, 0.0, 1.0);
	vec3 b = clamp(texture2D(_radTex, vTexcoord-up).xyz, 0.0, 1.0);
	vec3 t = clamp(texture2D(_radTex, vTexcoord+up).xyz, 0.0, 1.0);
	
	// find horizontal gradient change
	vec3 hChange = abs((c-l)-(r-c));
	if (hChange.x > _threshold || hChange.y > _threshold || hChange.z > _threshold) {
		discard;
	}	

	// find vertical gradient change
	vec3 vChange = abs((c-b)-(t-c));
	if (vChange.x > _threshold || vChange.y > _threshold || vChange.z > _threshold) {
		discard;
	}

	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
