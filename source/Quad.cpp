#include "Quad.h"


Quad::Quad(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft,
	glm::vec3 reflectance) {

	setModel(bottomLeft, bottomRight, topLeft);

	setReflectance(reflectance);


	// initial texture data
	float *initialPixels = new float[3*RAD_TEX_WIDTH*RAD_TEX_HEIGHT];
	memset(initialPixels, 0, sizeof(initialPixels));

	// FOR TESTING!!!!!!!!!!!!
	for (int i=0; i<RAD_TEX_WIDTH; ++i) {
		for (int j=0; j<RAD_TEX_HEIGHT; ++j) {
			int base = 3*i*j;
			if ( (i/4 + j/4)%2==0 ) { 
				initialPixels[base] = 1.0f;
				initialPixels[base+1] = 1.0f;
				initialPixels[base+2] = 1.0f;
			} else {
				initialPixels[base] = 0.0f;
				initialPixels[base+1] = 0.0f;
				initialPixels[base+2] = 1.0f;
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


}

Quad::~Quad() {
	glDeleteTextures(1, &radiosityTex);
	glDeleteTextures(1, &residualTex);
	glDeleteTextures(1, &radiosityTexB);
	glDeleteTextures(1, &residualTexB);
}


void Quad::setModel(glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft) {
	
	glm::vec3 u = bottomRight - bottomLeft;
	glm::vec3 v = topLeft - bottomLeft;
	glm::vec3 n = glm::normalize(glm::cross(u, v));

	this->model = glm::mat4(	u[0],	v[0],	n[0],	bottomLeft[0],
						u[1],	v[1],	n[1],	bottomLeft[1],
						u[2],	v[2],	n[2],	bottomLeft[2],
						0.0f,	0.0f,	0.0f,	1.0f
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