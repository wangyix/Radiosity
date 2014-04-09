#pragma once

#include "glincludes.h"

#define RAD_TEX_WIDTH 16
#define RAD_TEX_HEIGHT 16

class Quad {

public:
	static const int numVertices;
	static const int numIndices;
	static const float positions[];
	static const float texcoords[];
	static const unsigned short indices[];

private:

	glm::vec3 position;	// bottom-left corner
	glm::vec3 u;
	glm::vec3 v;

	glm::vec3 reflectance;

	// pingpong buffers
	GLuint radiosityTex;
	GLuint residualTex;
	GLuint radiosityTexB;
	GLuint residualTexB;

public:

	Quad();
	Quad(const glm::vec3 &bottomLeft, const glm::vec3 &bottomRight, const glm::vec3 &topLeft, 
		const glm::vec3 &reflectance, const glm::vec3 &emittance); 
	~Quad();

	/*
	static int tessellate(Quad *quads, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft,
			int rows, int columns, glm::vec3 reflectance);

	static void buildWorldVerticesArray(Quad *quads, int numQuads, float *positions, int *quadIds);
	*/
	/*
	void setModel(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft);
	void setReflectance(glm::vec3 reflectance);
	const glm::mat4 &getModel() const;
	*/

	GLuint getRadiosityTex() const;

	glm::vec3 getPosition() const;
	glm::vec3 getU() const;
	glm::vec3 getV() const;

private:
	void init(const glm::vec3 &bottomLeft, const glm::vec3 &bottomRight, const glm::vec3 &topLeft, 
		const glm::vec3 &reflectance, const glm::vec3 &emittance);
};