#pragma once

#include "glincludes.h"
#include "Utils.h"

class SubdivideShaderInterface {

private:
	GLuint shaderProgram;
	GLuint vertShader;
	GLuint fragShader;

	GLint radTex;
	GLint resTex;

	GLuint vao;
	GLuint texcoordVbo;
	GLuint indexVbo;

	// framebuffer for rendering to
	GLuint fbo;

	// texture sampler object
	GLuint bilinearClampToEdgeSampler;

public:

	SubdivideShaderInterface();

	void init();

	void setUniforms(GLuint radTex, GLuint resTex);

	void draw(	GLuint radTexBL, GLuint resTexBL,
				GLuint radTexBR, GLuint resTexBR,
				GLuint radTexTL, GLuint resTexTL,
				GLuint radTexTR, GLuint resTexTR,
				int texWidth, int texHeight);

	void close();
};