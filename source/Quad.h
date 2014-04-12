#pragma once

#include "glincludes.h"
#include <vector>

#define RAD_TEX_WIDTH 16
#define RAD_TEX_HEIGHT 16
#define RAD_TEX_TOPMIPLEVEL 4	// log2(RAD_TEX_DIMENSION)

class Quad {

private:
	static unsigned int nextId;
	static const float zeros[];

public:
	static const unsigned short indices[6];
	static const int numIndices;

private:

	unsigned int id;

	glm::vec3 position;	// bottom-left corner
	glm::vec3 u;
	glm::vec3 v;
	glm::vec3 n;

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

	glm::vec3 residualAvgIrradiance;	// cached


	int shooterRows;
	int shooterCols;
	std::vector<glm::vec3> residualShooterIrradiances;

public:

	Quad(const glm::vec3 &position	= glm::vec3(0.0f,0.0f,0.0f),
		const glm::vec3 &u			= glm::vec3(1.0f,0.0f,0.0f),
		const glm::vec3 &v			= glm::vec3(0.0f,1.0f,0.0f), 
		const glm::vec3 &reflectance	= glm::vec3(0.5f,0.5f,0.5f)); 
	
	void initTextures(const glm::vec3 &emittance);
	void closeTextures();

	
	void swapTextures();	// call after every reconstruction shader pass

	
	void selectAsShooter(int shooterLevel);


	void clearResidualTex();	// call on shooter at end of each shoot iteration
	
	/*
	void getShooterUniforms(int shooterLevel,	// substructuring info
			int shooterCol, int shooterRow,
			glm::mat4 *view, glm::vec3 *shooterPower) const;
	*/
	
	glm::vec3 getResidualAvgIrradiance() const;

	unsigned int getId() const;
	glm::vec3 getPosition() const;
	glm::vec3 getU() const;
	glm::vec3 getV() const;
	GLuint getRadiosityTex() const;
	GLuint getResidualTex() const;

private:

	void updateResidualAvgIrradiance();

};