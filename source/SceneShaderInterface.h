#pragma once

#include "glincludes.h"
#include "Utils.h"


class SceneShaderInterface {

private:
	GLuint shaderProgram;
	GLuint vertShader;
	GLuint fragShader;

	int numVertices;
	int numIndices;
	GLuint vao;
	GLuint positionVbo;
	GLuint texcoordVbo;
	GLuint indexVbo;

	GLint modelViewProj;
	GLint tex;

public:
	SceneShaderInterface();
	
	void init(int numVertices, const float *positions, const float *texcoords,
		int numIndices, const unsigned short *indices);
	void setModelViewProj(const glm::mat4 &modelViewProj);
	void setTexture(GLuint texture);
	void draw(int baseVertex);
	void close();
};