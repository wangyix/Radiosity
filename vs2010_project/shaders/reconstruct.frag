#version 400

// constants
uniform uint _id;				// ID of receiver quad								// checked
uniform vec3 _reflectance;		// color of receiver quad							// checked

uniform usampler2D _visTex;		// texture of IDs from visibility pass
uniform vec2 _visTexelSize;		// offset to use when doing PCF of visTex

uniform vec3 _shooterPower;		// (residual) shooter irradiance * shooter area		// checked
uniform vec3 _normal;			// normal of receiver in shooter viewspace			// checked

uniform sampler2D _radTex;		// radiosity texture of receiver					// checked
uniform sampler2D _resTex;		// residual texture of receiver						// checked


in vec3 vPosition;		// in shooter viewspace						// checked
in vec2 vTexcoord;													// checked

layout(location=0) out vec4 fNewRad;
layout(location=1) out vec4 fNewRes;


void main(void) {
	
	// get distance of receiver from shooter
	float d = length(vPosition);
	vec3 r = vPosition / d;		// direction to receiver texel

	// hemisphere projection
	//vec2 hemiNdc = r.xy;
	vec2 hemiNdc = r.xy / (1.0-r.z);

	// convert ndc to texture coordinates [-1,1]->[0,1]
	vec2 visTexcoord = hemiNdc*0.5 + 0.5;


	// see if this receiver texel is visible
	float visibility = 0.0;
	/*
	// non-PCF:
	uint visId = texture2D(_visTex, visTexcoord).x;						// checked
	visibility = (visId == _id) ? 1.0 : 0.0;
	*/
	
	// PCF: if any sample is visible, then this texel is visible
	// 3x3 samples
	/*
	for (int i=-1; i<=1; i++) {
		for (int j=-1; j<=1; j++) {
			vec2 sampleTexcoord = visTexcoord
					+ i * _visTexelSize.x + j * _visTexelSize.y;
			uint visId = texture2D(_visTex, sampleTexcoord).x;
			visibility += (visId == _id) ? 1.0 : 0.0;
		}
	}*/
	
	// PCF: 5x5 outer ring sampling
	vec2 sampleTexcoord = visTexcoord - 2*_visTexelSize;
	for (int i=0; i<4; i++) {
		uint visId = texture2D(_visTex, sampleTexcoord).x;
		visibility += (visId == _id) ? 1.0 : 0.0;
		sampleTexcoord.t += _visTexelSize.t;
	}
	for (int i=0; i<4; i++) {
		uint visId = texture2D(_visTex, sampleTexcoord).x;
		visibility += (visId == _id) ? 1.0 : 0.0;
		sampleTexcoord.s += _visTexelSize.s;
	}
	for (int i=0; i<4; i++) {
		uint visId = texture2D(_visTex, sampleTexcoord).x;
		visibility += (visId == _id) ? 1.0 : 0.0;
		sampleTexcoord.t -= _visTexelSize.t;
	}
	for (int i=0; i<4; i++) {
		uint visId = texture2D(_visTex, sampleTexcoord).x;
		visibility += (visId == _id) ? 1.0 : 0.0;
		sampleTexcoord.s -= _visTexelSize.s;
	}
	visibility = min(visibility, 1.0);
	

	// calculate form factor: cosi*cosj/(pi*d^2)
	float cosi = max(-dot(_normal, r), 0.0);
	float cosj = max(-r.z, 0.0);	// shooter normal in shooter viewspace is simply (0,0,-1)
	const float pi = 3.1415926536;
	float fij = cosi*cosj / (pi*d*d);

	// calculate received irradiance at this texel
	vec3 delta = visibility * _shooterPower * _reflectance * fij;

	// get radiosity and residual values of this texel
	vec3 rad = texture2D(_radTex, vTexcoord).xyz;
	vec3 res = texture2D(_resTex, vTexcoord).xyz;

	// add to radiosity and residual values
	fNewRad = vec4(rad + delta, 0.0);
	fNewRes = vec4(res + delta, 0.0);
}
