#version 400

// outputs quad patches.  That means tcs is invoked 4 times per patch
layout(vertices = 4) out;


in vec3 vPosition[];
in uint vQuadId[];
in float vTessLevel[];

out vec3 tcPosition[];
out uint tcQuadId[];

// built-in variables:
//in int gl_InvocationID;
//patch out float gl_TessLevelOuter[4];
//patch out float gl_TessLevelInner[2];


#define i gl_InvocationID

void main() {

	tcPosition[i] = vPosition[i];
	tcQuadId[i] = vQuadId[i];

	// set per-patch attributes once
	if (i==0) {
		gl_TessLevelOuter[0] = max(vTessLevel[1], vTessLevel[2]);	// between vertices 1 and 2
		gl_TessLevelOuter[1] = max(vTessLevel[2], vTessLevel[3]);	// between vertices 2 and 3
		gl_TessLevelOuter[2] = max(vTessLevel[3], vTessLevel[0]);	// between vertices 3 and 0
		gl_TessLevelOuter[3] = max(vTessLevel[0], vTessLevel[1]);	// between vertices 0 and 1
		gl_TessLevelInner[0] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);		// in direction of edges 0-1 and 2-3
		gl_TessLevelInner[1] = gl_TessLevelInner[0];								// in direction of edges 1-2 and 3-0
	}
}
