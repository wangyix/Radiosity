#pragma once

#include "glincludes.h"
#include "Utils.h"

class GradientShaderInterface {

private:
	GLuint shaderProgram;
	GLuint vertShader;
	GLuint fragShader;

	GLint radTexelSize;
	GLint threshold;
	GLint radTex;

	GLuint vao;
	GLuint texcoordVbo;
	GLuint indexVbo;

	// query object
	GLuint query;

	// texture sampler object
	GLuint nearestSampler;

public:
	GradientShaderInterface();

	void init();

	void setThresholdAndRadTexelSize(float threshold, float du, float dv);

	void setTexture(GLuint radTex);

	int draw(int texWidth, int texHeight);

	void close();
};