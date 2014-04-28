#pragma once

#include "glincludes.h"
#include "Utils.h"


class SceneShaderInterface {

private:
	GLuint shaderProgram;
	GLuint vertShader;
	GLuint fragShader;
	
	GLint modelViewProj;
	GLint tex;

	int numVertices;
	int numIndices;
	GLuint vao;
	GLuint positionVbo;
	GLuint texcoordVbo;
	GLuint indexVbo;

	// texture sampler object
	GLuint nearestClampToEdgeSampler;

public:
	SceneShaderInterface();
	
	void init(int numVertices, const float *positions, const float *texcoords,
		int numIndices, const unsigned short *indices);
	void setModelViewProj(const glm::mat4 &modelViewProj);
	void setTexture(GLuint texture);
	void draw(int baseVertex);
	void close();
};