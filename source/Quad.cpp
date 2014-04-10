#include "Quad.h"

const unsigned short Quad::indices[] = {0,1,2,0,2,3};
const int Quad::numIndices = 6;



Quad::Quad(const glm::vec3 &bottomLeft, const glm::vec3 &bottomRight, const glm::vec3 &topLeft, 
		const glm::vec3 &reflectance) {
	position = bottomLeft;
	u = bottomRight - bottomLeft;
	v = topLeft - bottomLeft;
	this->reflectance = reflectance;
}



void Quad::initTextures(const glm::vec3 &emittance) {

	// initial texture data
	float *initialPixels = new float[3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT];
	/*
	int k = 0;
	for (int i=0; i<RAD_TEX_HEIGHT; ++i) {
		for (int j=0; j<RAD_TEX_WIDTH; ++j) {
			initialPixels[k++] = emittance.x;
			initialPixels[k++] = emittance.y;
			initialPixels[k++] = emittance.z;
		}
	}*/

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


void Quad::closeTextures() {
	glDeleteTextures(1, &radiosityTex);
	glDeleteTextures(1, &residualTex);
	glDeleteTextures(1, &radiosityTexB);
	glDeleteTextures(1, &residualTexB);
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



GLuint Quad::getRadiosityTex() const {
	return radiosityTex;
}
