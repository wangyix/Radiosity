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
	
	glm::mat4 model;
	glm::vec3 reflectance;

	// pingpong buffers
	GLuint radiosityTex;
	GLuint residualTex;
	GLuint radiosityTexB;
	GLuint residualTexB;

public:

	Quad(glm::vec3 bottomLeft = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 bottomRight = glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3 topLeft = glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3 reflectance = glm::vec3(0.5f, 0.5f, 0.5f));

	~Quad();

	static int tessellate(Quad *quads, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft,
			int rows, int columns, glm::vec3 reflectance);

	void setModel(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft);
	void setReflectance(glm::vec3 reflectance);

	const glm::mat4 &getModel() const;
	GLuint getRadiosityTex() const;
};