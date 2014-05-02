#include "Quad.h"

const int Quad::numVertices = 4;
const int Quad::numIndices = 6;

const float Quad::positions[12] =  {	0.0f, 0.0f, 0.0f,
										1.0f, 0.0f, 0.0f,
										1.0f, 1.0f, 0.0f,
										0.0f, 1.0f, 0.0f };

const float Quad::texcoords[8] =  {	0.0, 0.0f,
									1.0, 0.0f,
									1.0, 1.0f,
									0.0, 1.0f };

const unsigned short Quad::indices[6] = {	0, 1, 2,
											0, 2, 3 };



Quad::Quad(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft,
	glm::vec3 reflectance) {

	setModel(bottomLeft, bottomRight, topLeft);

	setReflectance(reflectance);


	// initial texture data
	float *initialPixels = new float[3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT];
	memset(initialPixels, 0, sizeof(initialPixels));


	// FOR TESTING!!!!!!!!!!!!
	static float t = -0.2f;
	t += 0.01f;
	for (int i=0; i<RAD_TEX_HEIGHT; ++i) {
		for (int j=0; j<RAD_TEX_WIDTH; ++j) {
			int base = 3 * (i*RAD_TEX_WIDTH + j);
			if ( (i/4 + j/4)%2==0 ) { 
				initialPixels[base] = 1.0f;
				initialPixels[base+1] = 1.0f;
				initialPixels[base+2] = 1.0f;
			} else {
				initialPixels[base] = t;
				initialPixels[base+1] = 0.0;
				initialPixels[base+2] = 0.0f;
			}
		}
	}

	// TODO: create mipmaps for residual textures



	// allocate texture radiosityTex
	glGenTextures(1, &radiosityTex);
	glBindTexture(GL_TEXTURE_2D, radiosityTex);
	// filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// no mipmaps required
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// wrap params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// create texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,
		0, GL_RGB, GL_FLOAT, initialPixels);


	// allocate texture residualTex
	glGenTextures(1, &residualTex);
	glBindTexture(GL_TEXTURE_2D, residualTex);
	// filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// no mipmaps required
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// wrap params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// create texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,
		0, GL_RGB, GL_FLOAT, initialPixels);


	// allocate texture radiosityTexB
	glGenTextures(1, &radiosityTexB);
	glBindTexture(GL_TEXTURE_2D, radiosityTexB);
	// filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// no mipmaps required
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// wrap params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// create texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,
		0, GL_RGB, GL_FLOAT, initialPixels);


	// allocate texture residualTexB
	glGenTextures(1, &residualTexB);
	glBindTexture(GL_TEXTURE_2D, residualTexB);
	// filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// no mipmaps required
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// wrap params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// create texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RAD_TEX_WIDTH, RAD_TEX_HEIGHT,
		0, GL_RGB, GL_FLOAT, initialPixels);


	delete initialPixels;
}

Quad::~Quad() {
	glDeleteTextures(1, &radiosityTex);
	glDeleteTextures(1, &residualTex);
	glDeleteTextures(1, &radiosityTexB);
	glDeleteTextures(1, &residualTexB);
}



int Quad::tessellate(Quad *quads, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft,
int rows, int columns, glm::vec3 reflectance) {

	glm::vec3 u = (bottomRight-bottomLeft) / (float)columns;
	glm::vec3 v = (topLeft-bottomLeft) / (float)rows;

	glm::vec3 rowBase = bottomLeft;
	glm::vec3 base;
	int k = 0;
	for (int i=0; i<rows; ++i) {
		base = rowBase;
		for (int j=0; j<columns; ++j) {
			quads[k].setModel(base, base+u, base+v);
			quads[k].setReflectance(reflectance);
			k++;
			base += u;
		}
		rowBase += v;
	}
	return k;
}


void Quad::buildWorldVerticesArray(Quad *quads, int numQuads, float *positions, unsigned int *quadIds) {
	
	glm::vec4 posModel0 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 posModel1 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 posModel2 = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 posModel3 = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	int p = 0;
	int q = 0;

	glm::vec4 posWorld0, posWorld1, posWorld2, posWorld3;
	
	for (int i=0; i<numQuads; i++) {

		posWorld0 = quads[i].getModel() * posModel0;
		posWorld1 = quads[i].getModel() * posModel1;
		posWorld2 = quads[i].getModel() * posModel2;
		posWorld3 = quads[i].getModel() * posModel3;

		positions[p++] = posWorld0.x;
		positions[p++] = posWorld0.y;
		positions[p++] = posWorld0.z;
		quadIds[q++] = i+1;

		positions[p++] = posWorld1.x;
		positions[p++] = posWorld1.y;
		positions[p++] = posWorld1.z;
		quadIds[q++] = i+1;

		positions[p++] = posWorld2.x;
		positions[p++] = posWorld2.y;
		positions[p++] = posWorld2.z;
		quadIds[q++] = i+1;

		positions[p++] = posWorld3.x;
		positions[p++] = posWorld3.y;
		positions[p++] = posWorld3.z;
		quadIds[q++] = i+1;
	}
}


void Quad::setModel(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft) {
	
	glm::vec3 u = bottomRight - bottomLeft;
	glm::vec3 v = topLeft - bottomLeft;
	glm::vec3 n = glm::normalize(glm::cross(u, v));

	// constructor order is tranposed
	model = glm::mat4(	u.x,	u.y,	u.z,	0.0f,
						v.x,	v.y,	v.z,	0.0f,
						n.x,	n.y,	n.z,	0.0f,
						bottomLeft.x,	bottomLeft.y,	bottomLeft.z,	1.0f
					);
}

void Quad::setReflectance(glm::vec3 reflectance) {
	this->reflectance = reflectance;
}

const glm::mat4 &Quad::getModel() const {
	return model;
}

GLuint Quad::getRadiosityTex() const {
	return radiosityTex;
}