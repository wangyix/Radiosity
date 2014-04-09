#version 400

// outputs quad patches.  That means tcs is invoked 4 times per patch
layout(vertices = 4) out;


in vec3 vPosition[];
in int vQuadId[];
in float vTessLevel[];

out vec3 tcPosition[];
out int tcQuadId[];

// built-in variables:
//in int gl_InvocationID;
//patch out float gl_TessLevelOuter[4];
//patch out float gl_TessLevelInner[2];


#define i gl_InvocationID

void main() {

	tcPosition[i] = vPosition[i];

	// set per-patch attributes once
	if (i==0) {
		tcQuadId[0] = vQuadId[0];
		gl_TessLevelInner[0] = 1.0f;
		gl_TessLevelInner[1] = 1.0f;
		gl_TessLevelOuter[0] = max(vTessLevel[1], vTessLevel[2]);
		gl_TessLevelOuter[1] = max(vTessLevel[0], vTessLevel[1]);
		gl_TessLevelOuter[2] = max(vTessLevel[3], vTessLevel[0]);
		gl_TessLevelOuter[3] = max(vTessLevel[2], vTessLevel[3]);
	}
}
