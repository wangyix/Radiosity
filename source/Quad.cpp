#include "Quad.h"

unsigned int Quad::nextId = 1;
const float Quad::zeros[3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT] = {0.0f};

const unsigned short Quad::indices[] = {0,1,2,0,2,3};
const int Quad::numIndices = 6;



Quad::Quad(const glm::vec3 &position, const glm::vec3 &u, const glm::vec3 &v, 
		const glm::vec3 &reflectance) 
		:  currentRadiosityTex(0), nextRadiosityTex(0),
		currentResidualTex(0), nextResidualTex(0),
		shooterRows(0), shooterCols(0),
		residualShooterIrradiances(),
		currentShooterRow(0), currentShooterCol(0) {

	id = nextId++;
	this->position = position;
	this->u = u;
	this->v = v;
	n = glm::normalize(glm::cross(u, v));
	this->reflectance = reflectance;
}



void Quad::initTextures(const glm::vec3 &emittance) {

	// initial texture data
	float *initialPixels = new float[3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT];
	
	int k = 0;
	for (int i=0; i<RAD_TEX_HEIGHT; ++i) {
		for (int j=0; j<RAD_TEX_WIDTH; ++j) {
			initialPixels[k++] = emittance.x;
			initialPixels[k++] = emittance.y;
			initialPixels[k++] = emittance.z;
		}
	}
	
	/*
	// FOR TESTING!!!!!!!!!!!!
	static float t = 0.0f;
	for (int i=0; i<RAD_TEX_HEIGHT; ++i) {
		for (int j=0; j<RAD_TEX_WIDTH; ++j) {
			int base = 3 * (i*RAD_TEX_WIDTH + j);
			if ( (i/4 + j/4)%2==0 ) { 
				initialPixels[base] = 0.0f;
				initialPixels[base+1] = 0.0f;
				initialPixels[base+2] = 1.0f;
			} else {
				initialPixels[base] = emittance.x;
				initialPixels[base+1] = emittance.y;
				initialPixels[base+2] = emittance.z;
			}
		}
	}*/


	// initialize radiosity and residual texture pairs

	glGenTextures(1, &currentRadiosityTex);
	glBindTexture(GL_TEXTURE_2D, currentRadiosityTex);
	// set texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// create texture with mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,	// mutable storage
		0, GL_RGB, GL_FLOAT, initialPixels);
	glGenerateMipmap(GL_TEXTURE_2D);


	glGenTextures(1, &nextRadiosityTex);
	glBindTexture(GL_TEXTURE_2D, nextRadiosityTex);
	// set texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// create texture with mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,
		0, GL_RGB, GL_FLOAT, initialPixels);
	glGenerateMipmap(GL_TEXTURE_2D);


	glGenTextures(1, &currentResidualTex);
	glBindTexture(GL_TEXTURE_2D, currentResidualTex);
	// set texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// create texture with mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,
		0, GL_RGB, GL_FLOAT, initialPixels);
	glGenerateMipmap(GL_TEXTURE_2D);


	glGenTextures(1, &nextResidualTex);
	glBindTexture(GL_TEXTURE_2D, nextResidualTex);
	// set texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// create texture with mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,
		0, GL_RGB, GL_FLOAT, initialPixels);
	glGenerateMipmap(GL_TEXTURE_2D);


	delete[] initialPixels;
}


void Quad::closeTextures() {
	glDeleteTextures(1, &currentRadiosityTex);
	glDeleteTextures(1, &nextRadiosityTex);
	glDeleteTextures(1, &currentResidualTex);
	glDeleteTextures(1, &nextResidualTex);
}


glm::vec3 Quad::getResidualAvgIrradiance() const {
	
	glBindTexture(GL_TEXTURE_2D, currentResidualTex);

	glm::vec3 residualAvgIrradiance;
	glGetTexImage(GL_TEXTURE_2D, RAD_TEX_TOPMIPLEVEL, GL_RGB, GL_FLOAT, 
			glm::value_ptr(residualAvgIrradiance));
	
	return residualAvgIrradiance;
}




void Quad::swapTextures() {

	GLuint temp;
	
	temp = currentRadiosityTex;
	currentRadiosityTex = nextRadiosityTex;
	nextRadiosityTex = temp;

	temp = currentResidualTex;
	currentResidualTex = nextResidualTex;
	nextResidualTex = temp;
}



void Quad::selectAsShooter(int shooterDensity) {
	
	int shooterLevel = RAD_TEX_TOPMIPLEVEL - shooterDensity;
	shooterRows = RAD_TEX_WIDTH >> shooterLevel;
	shooterCols = RAD_TEX_HEIGHT >> shooterLevel;

	currentShooterRow = 0;
	currentShooterCol = 0;

	// read appropriate mipmap of the residual texture

	glBindTexture(GL_TEXTURE_2D, currentResidualTex);

	residualShooterIrradiances.resize(shooterRows*shooterCols);
	glGetTexImage(GL_TEXTURE_2D, shooterLevel, GL_RGB, GL_FLOAT,
			glm::value_ptr(residualShooterIrradiances[0]));
}


bool Quad::hasNextShooterCell() const {
	return (currentShooterRow < shooterRows);
}


void Quad::getNextShooterCellUniforms(glm::mat4 *view_ptr,
	glm::vec3 *shooterPower_ptr) {


	// calculate shooter position
	glm::vec3 shooterPosition = position;
	shooterPosition += (currentShooterCol+0.5f) * (u / (float)shooterCols);
	shooterPosition += (currentShooterRow+0.5f) * (v / (float)shooterRows);

	// calculate normalized u,v for camera axes
	glm::vec3 right = glm::normalize(v);
	glm::vec3 up = glm::normalize(u);
	glm::vec3 lookNeg = -n;
	
	// compute view matrix
	float tR = -glm::dot(shooterPosition, right);
	float tU = -glm::dot(shooterPosition, up);
	float tL = -glm::dot(shooterPosition, lookNeg);
	// constructor order is tranposed 
	*view_ptr = glm::mat4(	right.x,	up.x,	lookNeg.x,	0.0f,
							right.y,	up.y,	lookNeg.y,	0.0f,
							right.z,	up.z,	lookNeg.z,	0.0f,
							tR,			tU,		tL,			1.0f );


	// compute shooter power (area * irradiance)
	int k = currentShooterRow * shooterCols + currentShooterCol;
	*shooterPower_ptr = glm::length(u) / (float)shooterCols
					* glm::length(v) / (float)shooterRows
					* residualShooterIrradiances[k];

	
	if (currentShooterCol == shooterCols-1) {
		currentShooterCol = 0;
		currentShooterRow++;
	} else {
		currentShooterCol++;
	}
}


void Quad::clearResidualTex() {

	glBindTexture(GL_TEXTURE_2D, currentResidualTex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,	// mutable storage
		0, GL_RGB, GL_FLOAT, zeros);

	// rebuild mipmaps of residual texture
	glGenerateMipmap(GL_TEXTURE_2D);
}






unsigned int Quad::getId() const {
	return id;
}

glm::vec3 Quad::getPosition() const {
	return position;
}

glm::vec3 Quad::getU() const {
	return u;
}

glm::vec3 Quad::getV() const {
	return v;
}

glm::vec3 Quad::getN() const {
	return n;
}

GLuint Quad::getRadiosityTex() const {
	return currentRadiosityTex;
}

GLuint Quad::getResidualTex() const {
	return currentResidualTex;
}

GLuint Quad::getNextRadiosityTex() const {
	return nextRadiosityTex;
}

GLuint Quad::getNextResidualTex() const {
	return nextResidualTex;
}

glm::vec3 Quad::getReflectance() const {
	return reflectance;
}

int Quad::getTexWidth() {
	return RAD_TEX_WIDTH;
}

int Quad::getTexHeight() {
	return RAD_TEX_HEIGHT;
}



void Quad::printRadTex() {

	// TEST!!! read back new residual texture
	printf("\n\n\n\n");
	glBindTexture(GL_TEXTURE_2D, currentRadiosityTex);
	float *irr = new float [3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, irr);
	for (int i=0; i<RAD_TEX_HEIGHT; i++) {
		for (int j=0; j<RAD_TEX_WIDTH; j++) {
			int base = 3*(i*RAD_TEX_WIDTH+j);
			printf(" (%3.3f %3.3f %3.3f)", irr[base], irr[base+1], irr[base+2]);
		}
		printf("\n");
	}
	delete[] irr;
}


void Quad::printResTex() {

	// TEST!!! read back new residual texture
	printf("\n\n\n\n");
	glBindTexture(GL_TEXTURE_2D, currentResidualTex);
	float *irr = new float [3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, irr);
	for (int i=0; i<RAD_TEX_HEIGHT; i++) {
		for (int j=0; j<RAD_TEX_WIDTH; j++) {
			int base = 3*(i*RAD_TEX_WIDTH+j);
			printf(" (%3.3f %3.3f %3.3f)", irr[base], irr[base+1], irr[base+2]);
		}
		printf("\n");
	}
	delete[] irr;
}