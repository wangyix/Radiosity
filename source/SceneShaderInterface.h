#pragma once

#include "glincludes.h"
#include "ShaderLoader.h"

#include "QuadMesh.h"	// REMOVE LATER!!!!!

class SceneShaderInterface {
//private:
public:
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
	
	int init();
	void setModelViewProj(const glm::mat4 &modelViewProj);
	void draw();
	void close();
	

private:

	void setVertices(int numVertices, const float *positions, const float *texcoords,
		int numIndices, const unsigned short *indices);

	void bindProgram();
	
	void setTexture(GLuint texture);
	

	
	
};