#version 400

// outputs quad patches.  That means tcs is invoked 4 times per patch
layout(vertices = 4) out;


in vec3 vPosition[];
in uint vQuadId[];

out vec3 tcPosition[];
out uint tcQuadId[];



// built-in variables:
//in int gl_InvocationID;
//patch out float gl_TessLevelOuter[4];
//patch out float gl_TessLevelInner[2];


#define i gl_InvocationID


#define MAX_TESS_LEVEL 64.0f
#define MIN_TESS_LEVEL 1.0f

#define DIST_FOR_MIN_TESS 5.0f


void main() {

	tcPosition[i] = vPosition[i];
	tcQuadId[i] = vQuadId[i];
	

	// calculate outer tess level i
	// figure out the two positions that the tess level applies to
	vec3 a = vPosition[(i+1)&3];		// & 3 same as % 4
	vec3 b = vPosition[(i+2)&3];
	vec3 ab = b - a;
	float abLength = length(ab);

	// project origin onto line ab, clamp it within segment ab
	// x = a+u(b-a)
	float u = clamp(-dot(a, ab)/dot(ab, ab), 0.0, 1.0);

	// calculate distance of closest point on ab to the origin
	vec3 abClosest = a + u*ab;
	float d = length(abClosest);


	// calculate the tess level of this edge using that distance
	// and the length of the edge
	float tessLevel = abLength * (DIST_FOR_MIN_TESS / d)*MIN_TESS_LEVEL;
	gl_TessLevelOuter[i] = clamp(tessLevel, MIN_TESS_LEVEL, MAX_TESS_LEVEL);



	// set inner tess levels
	
	if (i==0) {

		// NOTE: inside this if, we know that a is vPosition[1] and b is vPosition[2]
		// so ab is edge 1-2 and ac is edge 1-0
		
		// get position c so we ab and ac form a basis of this quad
		vec3 c = vPosition[0];
		vec3 ac = c - a;
		float acLength = length(ac);

		// project origin onto line ac, clamp it within segment ac
		// y = a+v*(c-a)
		float v = clamp(-dot(a, ac)/dot(ac, ac), 0.0, 1.0);

		// calculate distance of closest point on quad to origin
		float d2 = length(abClosest + v*ac);

		// calculate the tess level before scaling for edge length
		float unscaledTessLevel = (DIST_FOR_MIN_TESS / d2)*MIN_TESS_LEVEL;

		// calculate final inner tess levels, scaling by length of appropriate edge
		gl_TessLevelInner[0] = clamp(acLength*unscaledTessLevel, MIN_TESS_LEVEL, MAX_TESS_LEVEL);	// in direction of edges 0-1 and 2-3
		gl_TessLevelInner[1] = clamp(abLength*unscaledTessLevel, MIN_TESS_LEVEL, MAX_TESS_LEVEL);	// in direction of edges 1-2 and 3-0
	}
}


/*
// between vertices 1 and 2
float level = distance(vPosition[1], vPosition[2]) * max(vTessLevel[1], vTessLevel[2]);
gl_TessLevelOuter[0] = clamp(level, MIN_TESS_LEVEL, MAX_TESS_LEVEL);
// between vertices 2 and 3
level = distance(vPosition[2], vPosition[3]) * max(vTessLevel[2], vTessLevel[3]);
gl_TessLevelOuter[1] = clamp(level, MIN_TESS_LEVEL, MAX_TESS_LEVEL);;
// between vertices 3 and 0
level = distance(vPosition[3], vPosition[0]) * max(vTessLevel[3], vTessLevel[0]);
gl_TessLevelOuter[2] = clamp(level, MIN_TESS_LEVEL, MAX_TESS_LEVEL);
// between vertices 0 and 1
level = distance(vPosition[0], vPosition[1]) * max(vTessLevel[0], vTessLevel[1]);
gl_TessLevelOuter[3] = clamp(level, MIN_TESS_LEVEL, MAX_TESS_LEVEL);
*/