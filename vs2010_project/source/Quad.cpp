#include "Quad.h"

unsigned int Quad::nextId = 1;
const float Quad::zeros[3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT] = {0.0f};

const float Quad::positionsModel[] = {	0.0f, 0.0f, 0.0f,
										1.0f, 0.0f, 0.0f,
										1.0f, 1.0f, 0.0f,
										0.0f, 1.0f, 0.0f	};
const float Quad::texcoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
const int Quad::numVertices = 4;

const unsigned short Quad::indices[] = {0,1,2,0,2,3};
const int Quad::numIndices = 6;



Quad::Quad() 
		:  currentRadiosityTex(0), nextRadiosityTex(0),
		currentResidualTex(0), nextResidualTex(0),
		shooterRows(0), shooterCols(0),
		residualShooterIrradiances(),
		currentShooterRow(0), currentShooterCol(0) {

	id = nextId++;
	parentId = id;
}




void Quad::updateNormalAndModel() {

	n = glm::normalize(glm::cross(u, v));

	// update model matrix (glm mat constructor order is tranposed)
	model = glm::mat4(	u.x,		u.y,		u.z,		0.0f,
						v.x,		v.y,		v.z,		0.0f,
						n.x,		n.y,		n.z,		0.0f,
						position.x,	position.y,	position.z,	1.0f
					);
}

void Quad::setProperties(const glm::vec3 &position, const glm::vec3 &u,
		const glm::vec3 &v, const glm::vec3 &reflectance) {

	this->position = position;
	this->u = u;
	this->v = v;
	this->reflectance = reflectance;

	updateNormalAndModel();
}





void Quad::init(const glm::vec3 &position, const glm::vec3 &u,
		const glm::vec3 &v, const glm::vec3 &reflectance,
		const glm::vec3 &emittance) {

	setProperties(position, u, v, reflectance);

	this->subdivideLevel = 0;

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


	initTextures(initialPixels);

	delete[] initialPixels;
}



void Quad::init(const glm::vec3 &position, const glm::vec3 &u,
		const glm::vec3 &v, const glm::vec3 &reflectance,
		unsigned int parentId, int subdivideLevel) {

	setProperties(position, u, v, reflectance);

	this->parentId = parentId;
	this->subdivideLevel = subdivideLevel;

	initTextures(0);
}



void Quad::initTextures(float *initialPixels) {

	// initialize radiosity and residual texture pairs

	glGenTextures(1, &currentRadiosityTex);
	glBindTexture(GL_TEXTURE_2D, currentRadiosityTex);
	setupCurrentTexture(initialPixels);


	glGenTextures(1, &nextRadiosityTex);
	glBindTexture(GL_TEXTURE_2D, nextRadiosityTex);
	setupCurrentTexture(initialPixels);


	glGenTextures(1, &currentResidualTex);
	glBindTexture(GL_TEXTURE_2D, currentResidualTex);
	setupCurrentTexture(initialPixels);


	glGenTextures(1, &nextResidualTex);
	glBindTexture(GL_TEXTURE_2D, nextResidualTex);
	setupCurrentTexture(initialPixels);
}


void Quad::setupCurrentTexture(float *initialPixels) {

	// create texture with mipmaps
	
	// immutable storage!!!
	glTexStorage2D(GL_TEXTURE_2D, RAD_TEX_TOPMIPLEVEL+1, GL_RGB32F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RAD_TEX_WIDTH, RAD_TEX_HEIGHT, GL_RGB, GL_FLOAT, initialPixels);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,	// mutable storage
		//0, GL_RGB, GL_FLOAT, initialPixels);

	glGenerateMipmap(GL_TEXTURE_2D);

	/*
	// set texture params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	*/
}



void Quad::close() {
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
	
	shooterDensity = std::max(shooterDensity, 0);
	int shooterLevel = std::max(RAD_TEX_TOPMIPLEVEL-shooterDensity, 0);
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

	// immutable storage!!
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RAD_TEX_WIDTH, RAD_TEX_HEIGHT, GL_RGB, GL_FLOAT, zeros);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,	// mutable storage
		//0, GL_RGB, GL_FLOAT, zeros);

	// rebuild mipmaps of residual texture
	glGenerateMipmap(GL_TEXTURE_2D);
}



void Quad::setU(const glm::vec3 &u) {
	this->u = u;
	updateNormalAndModel();
}

void Quad::setV(const glm::vec3 &v) {
	this->v = v;
	updateNormalAndModel();
}

void Quad::setSubdivideLevel(int subdivideLevel) {
	this->subdivideLevel = subdivideLevel;
}

unsigned int Quad::getId() const {
	return id;
}

unsigned int Quad::getParentId() const {
	return parentId;
}

int Quad::getSubdivideLevel() const {
	return subdivideLevel;
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

const glm::mat4 &Quad::getModel() const {
	return model;
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

int Quad::getCurrentShooterRow() const {
	return currentShooterRow;
}

int Quad::getCurrentShooterCol() const {
	return currentShooterCol;
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

void Quad::printNextRadTex() {

	// TEST!!! read back new residual texture
	printf("\n\n\n\n");
	glBindTexture(GL_TEXTURE_2D, nextRadiosityTex);
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


void Quad::printNextResTex() {

	// TEST!!! read back new residual texture
	printf("\n\n\n\n");
	glBindTexture(GL_TEXTURE_2D, nextResidualTex);
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