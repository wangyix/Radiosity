#version 400

// constants
uniform mat4 _modelViewProj;

// vertex shader input
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;

// vertex shader output
out vec2 uv;


void main(void) {
	// compute position
	gl_Position = _modelViewProj * vec4(position, 1.0);

	//gl_Position = vec4(position, 1.0);
	uv = texcoord;	
}


/*
#version 400

layout(location=0) in vec3 in_Position;
layout(location=1) in vec2 in_Color;
out vec4 ex_Color;

void main(void)
{
	gl_Position = vec4(in_Position, 1);
	ex_Color = vec4(in_Color, 0, 1);
}*/