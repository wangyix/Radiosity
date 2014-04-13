#pragma once

#include "glincludes.h"
#include "Utils.h"
#include <vector>

#define RAD_TEX_WIDTH 512
#define RAD_TEX_HEIGHT 512
#define RAD_TEX_TOPMIPLEVEL 9	// log2(RAD_TEX_DIMENSION)

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
	
	GLuint currentResidualTex;
	GLuint nextResidualTex;


	int shooterRows;
	int shooterCols;
	std::vector<glm::vec3> residualShooterIrradiances;

	int currentShooterRow;
	int currentShooterCol;


public:

	Quad(const glm::vec3 &position	= glm::vec3(0.0f,0.0f,0.0f),
		const glm::vec3 &u			= glm::vec3(1.0f,0.0f,0.0f),
		const glm::vec3 &v			= glm::vec3(0.0f,1.0f,0.0f), 
		const glm::vec3 &reflectance	= glm::vec3(0.5f,0.5f,0.5f)); 
	
	void initTextures(const glm::vec3 &emittance);
	void closeTextures();


	glm::vec3 getResidualAvgIrradiance() const;

	void selectAsShooter(int shooterDensity);

	bool hasNextShooterCell() const;
	void getNextShooterCellUniforms(glm::mat4 *view_ptr, glm::vec3 *shooterPower_ptr);

	void clearResidualTex();	// call on shooter at end of each shoot iteration
	
	
	void swapTextures();	// call after every reconstruction shader pass


	unsigned int getId() const;
	glm::vec3 getPosition() const;
	glm::vec3 getU() const;
	glm::vec3 getV() const;
	glm::vec3 getN() const;
	GLuint getRadiosityTex() const;
	GLuint getResidualTex() const;
	GLuint getNextRadiosityTex() const;
	GLuint getNextResidualTex() const;
	glm::vec3 getReflectance() const;
	
	static int getTexWidth();
	static int getTexHeight();


	// for testing
	void printRadTex();
	void printResTex();

};