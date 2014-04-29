#version 400

// constants

uniform sampler2D _radTex;		// radiosity texture of receiver
uniform sampler2D _resTex;		// residual texture of receiver


in vec2 vTexcoord;

layout(location=0) out vec4 fSubRadBL;
layout(location=1) out vec4 fSubResBL;
layout(location=2) out vec4 fSubRadBR;
layout(location=3) out vec4 fSubResBR;
layout(location=4) out vec4 fSubRadTL;
layout(location=5) out vec4 fSubResTL;
layout(location=6) out vec4 fSubRadTR;
layout(location=7) out vec4 fSubResTR;



void main(void) {
	
	vec2 texcoordBL = vTexcoord * 0.5;
	vec2 texcoordBR = texcoordBL + vec2(0.5, 0.0);
	vec2 texcoordTL = texcoordBL + vec2(0.0, 0.5);
	vec2 texcoordTR = texcoordBL + vec2(0.5, 0.5);

	// bottom-left quadrant
	fSubRadBL = vec4(1,0,0,1);//texture2D(_radTex, texcoordBL);
	fSubResBL = vec4(1,0,0,1);//texture2D(_resTex, texcoordBL);

	// bottom-right quadrant
	fSubRadBR = vec4(1,0,0,1);//texture2D(_radTex, texcoordBR);
	fSubResBR = vec4(1,0,0,1);//texture2D(_resTex, texcoordBR);

	// top-left quadrant
	fSubRadTL = texture2D(_radTex, texcoordTL);
	fSubResTL = texture2D(_resTex, texcoordTL);

	// top-right quadrant
	fSubRadTR = texture2D(_radTex, texcoordTR);
	fSubResTR = texture2D(_resTex, texcoordTR);
}
