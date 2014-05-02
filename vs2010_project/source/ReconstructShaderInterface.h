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
	GLint visTexelSize;
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

	// texture sampler objects
	GLuint nearestSampler;
	GLuint nearestClampToBorderSampler;

public:

	ReconstructShaderInterface();

	void init(int numVertices, const float *positions, const float *texcoords,
			int numIndices, const unsigned short *indices);
	
	void setVisTexelSize(float du, float dv);

	void setShooterUniforms(const glm::vec3 &shooterPower);

	void setReceiverUniforms(const glm::mat4 &modelView, unsigned int id,
							const glm::vec3 &reflectance,
							const glm::vec3 &normalShooterView);

	void setTextureUniforms(GLuint visibilityTex,
			GLuint radiosityTex, GLuint residualTex);

	void draw(GLuint nextRadTex, GLuint nextResTex,
			int texWidth, int texHeight);

	void close();
};