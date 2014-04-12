#pragma once

#include "glincludes.h"
#include "Utils.h"

class ReconstructShaderInterface {

private:
	GLuint shaderProgram;
	GLuint vertShader;
	GLuint fragShader;

	GLint modelView;
	GLint id;
	GLint reflectance;
	GLint shooterPower;
	GLint normal;
	GLint visTex;
	GLint radTex;
	GLint resTex;

	int numVertices;
	int numIndices;
	GLuint vao;
	GLuint positionVbo;
	GLuint texcoordVbo;
	GLuint indexVbo;

	// framebuffer for rendering to
	GLuint fbo;


public:

	ReconstructShaderInterface();

	void init(int numVertices, const float *positions, const float *texcoords,
			int numIndices, const unsigned short *indices);
	
	void setShooterUniforms(const glm::mat4 &modelView,
							const glm::vec3 &shooterPower,
							GLuint visibilityTexture);

	void setReceiverUniforms(unsigned int id,
							const glm::vec3 &reflectance,
							const glm::vec3 &normalShooterView,
							GLuint radTex, GLuint resTex);

	void draw(int baseVertex, GLuint nextRadTex, GLuint nextResTex,
			int texWidth, int texHeight);

	void close();
};