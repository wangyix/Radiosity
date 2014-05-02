#version 400

// input patches are quads, equal distance between patch vertices, output triangles wind ccw
layout(quads, equal_spacing, ccw) in;


uniform vec2 _nearFar;	// near and far planes


in vec3 tcPosition[];
in uint tcQuadId[];

out uint teQuadId;

// built-in variables:
//in vec3 gl_TessCoord;		// normalized coordinates of this patch point


void main() {

	teQuadId = tcQuadId[0];

	vec3 a = mix(tcPosition[1], tcPosition[0], gl_TessCoord.x);
	vec3 b = mix(tcPosition[2], tcPosition[3], gl_TessCoord.x);
	vec3 viewPos = mix(b, a, gl_TessCoord.y);
	
	// hemisphere projection
	
	vec3 hemiPos = normalize(viewPos);
	float fMinusN = _nearFar.y - _nearFar.x;
	//gl_Position.xy = hemiPos.xy * fMinusN;
	gl_Position.xy = hemiPos.xy / (1.0-hemiPos.z) * fMinusN;

	gl_Position.z = (-2.0*viewPos.z - _nearFar.x - _nearFar.y);
	gl_Position.w = fMinusN;
}
