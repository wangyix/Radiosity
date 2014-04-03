#version 400

// constants
uniform sampler2D _tex;

// fragment shader input
in vec2 uv;

void main(void) {
	//gl_FragColor = vec4(texture2D(_tex, uv).xyz, 1.0);
	gl_FragColor = vec4(uv, 0, 1);
}


/*
#version 400

in vec4 ex_Color;
out vec4 out_Color;

void main(void)
{
	out_Color = ex_Color;
}
*/