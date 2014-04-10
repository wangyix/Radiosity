#pragma once

#include "glincludes.h"

#define RAD_TEX_WIDTH 16
#define RAD_TEX_HEIGHT 16

class Quad {

public:
	static const unsigned short indices[6];
	static const int numIndices;

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

	Quad(const glm::vec3 &bottomLeft	= glm::vec3(0.0f,0.0f,0.0f),
		const glm::vec3 &bottomRight	= glm::vec3(1.0f,0.0f,0.0f),
		const glm::vec3 &topLeft		= glm::vec3(0.0f,1.0f,0.0f), 
		const glm::vec3 &reflectance	= glm::vec3(0.5f,0.5f,0.5f)); 
	
	void initTextures(const glm::vec3 &emittance);
	void closeTextures();

	GLuint getRadiosityTex() const;

	glm::vec3 getPosition() const;
	glm::vec3 getU() const;
	glm::vec3 getV() const;

};