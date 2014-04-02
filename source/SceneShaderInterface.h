#pragma once

#include "glincludes.h"
#include "ShaderLoader.h"


class SceneShaderInterface {
private:
	GLuint shaderProgram;

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
	
	int init();

	void setVertices(int numVertices, const float *positions, const float *texcoords,
		int numIndices, const unsigned short *indices);

	void bindProgram();
	void setModelViewProj(const glm::mat4 &modelViewProj);
	void setTexture(GLuint texture);
	void draw();

	void close();
};