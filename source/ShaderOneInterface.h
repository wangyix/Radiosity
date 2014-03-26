#pragma once

#include "glincludes.h"
#include "ShaderLoader.h"

class ShaderOneInterface {
private:
	GLuint shaderProgram;

	GLint modelViewProj;
	GLint modelView;
	GLint modelViewInvTrans;

public:
	ShaderOneInterface();
	
	int init();
	void close();

};