#pragma once

#include "glincludes.h"

#define RAD_TEX_WIDTH 16
#define RAD_TEX_HEIGHT 16

class Quad {

private:
	static unsigned int nextId;

public:
	static const unsigned short indices[6];
	static const int numIndices;

private:

	unsigned int id;

	glm::vec3 position;	// bottom-left corner
	glm::vec3 u;
	glm::vec3 v;

	glm::vec3 reflectance;

	// pingpong buffers
	GLuint currentRadiosityTex;
	GLuint nextRadiosityTex;
	GLuint radiosityTex;
	GLuint radiosityTexB;
	
	GLuint currentResidualTex;
	GLuint nextResidualTex;
	GLuint residualTex;
	GLuint residualTexB;

public:

	Quad(const glm::vec3 &bottomLeft	= glm::vec3(0.0f,0.0f,0.0f),
		const glm::vec3 &bottomRight	= glm::vec3(1.0f,0.0f,0.0f),
		const glm::vec3 &topLeft		= glm::vec3(0.0f,1.0f,0.0f), 
		const glm::vec3 &reflectance	= glm::vec3(0.5f,0.5f,0.5f)); 
	
	void initTextures(const glm::vec3 &emittance);
	void closeTextures();

	void swapTextures();	// call after every reconstruction shader pass

	glm::vec3 getResidualPower() const;

	GLuint getRadiosityTex() const;

	unsigned int getId() const;
	glm::vec3 getPosition() const;
	glm::vec3 getU() const;
	glm::vec3 getV() const;

};