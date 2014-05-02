#pragma once

#include "glincludes.h"
#include "Utils.h"
#include <vector>

#define RAD_TEX_WIDTH 16
#define RAD_TEX_HEIGHT 16
#define RAD_TEX_TOPMIPLEVEL 4	// log2(RAD_TEX_DIMENSION)

class Quad {

private:
	static unsigned int nextId;
	static const float zeros[];

public:
	static const float positionsModel[12];
	static const float texcoords[8];
	static const int numVertices;
	
	static const unsigned short indices[6];
	static const int numIndices;

private:

	unsigned int id;
	unsigned int parentId;

	int subdivideLevel;

	glm::vec3 position;	// bottom-left corner
	glm::vec3 u;
	glm::vec3 v;
	glm::vec3 n;
	glm::vec3 reflectance;

	glm::mat4 model;


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

	Quad();

	void init(const glm::vec3 &position, const glm::vec3 &u,
		const glm::vec3 &v, const glm::vec3 &reflectance,
		unsigned int parentId, int subdivideLevel);

	void init(const glm::vec3 &position, const glm::vec3 &u,
		const glm::vec3 &v, const glm::vec3 &reflectance,
		const glm::vec3 &emittance);

	void close();


	glm::vec3 getResidualAvgIrradiance() const;

	void selectAsShooter(int shooterDensity);

	bool hasNextShooterCell() const;
	void getNextShooterCellUniforms(glm::mat4 *view_ptr, glm::vec3 *shooterPower_ptr);

	void clearResidualTex();	// call on shooter at end of each shoot iteration
	
	
	void swapTextures();	// call after every reconstruction shader pass


	void setU(const glm::vec3 &u);
	void setV(const glm::vec3 &v);
	void setSubdivideLevel(int subdivideLevel);

	unsigned int getId() const;
	unsigned int getParentId() const;
	glm::vec3 getPosition() const;
	glm::vec3 getU() const;
	glm::vec3 getV() const;
	glm::vec3 getN() const;
	const glm::mat4 &getModel() const;
	GLuint getRadiosityTex() const;
	GLuint getResidualTex() const;
	GLuint getNextRadiosityTex() const;
	GLuint getNextResidualTex() const;
	glm::vec3 getReflectance() const;
	int getCurrentShooterRow() const;
	int getCurrentShooterCol() const;
	int getSubdivideLevel() const;


	static int getTexWidth();
	static int getTexHeight();


	// for testing
	void printRadTex();
	void printResTex();
	void printNextRadTex();
	void printNextResTex();

private:
	
	void setProperties(const glm::vec3 &position, const glm::vec3 &u,
			const glm::vec3 &v, const glm::vec3 &reflectance);
	void updateNormalAndModel();


	void initTextures(float *initialPixels);
	void setupCurrentTexture(float *initialPixels);
};