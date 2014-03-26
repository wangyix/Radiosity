#version 400

// constants
uniform sampler2D tex;

// fragment shader input
varying vec3 n;
varying vec2 uv;


void main(void)
{
    vec3 eyeSpaceLigthDirection = vec3(0.0,0.0,1.0);
    float diffuse = max(0.0,dot(normalize(n),eyeSpaceLigthDirection));
	gl_FragColor = vec4(texture2D(tex,uv).xyz*diffuse,1.0);
}