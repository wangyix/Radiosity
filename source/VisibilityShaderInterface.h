#pragma once

#include "glincludes.h"
#include "Utils.h"

#define VIS_BUFFER_WIDTH 512
#define VIS_BUFFER_HEIGHT 512


class VisibilityShaderInterface {

private:
	GLuint shaderProgram;
	GLuint vertShader;
	GLuint tessControlShader;
	GLuint tessEvalShader;
	GLuint fragShader;
	
	// uniform locations
	GLint modelView;
	GLint nearFar;

	int numVertices;
	GLuint vao;
	GLuint positionVbo;
	GLuint idVbo;

	// framebuffer for rendering to
	GLuint visFramebuffer;
	GLuint visTexture;
	GLuint visDepth;

public:
	
	VisibilityShaderInterface();
	
	void init(int numVertices, const float *positions, const unsigned int *ids);
	

	void setNearFar(float near, float far);
	void setModelView(const glm::mat4 &modelView);

	void draw();


	void renderVisibilityBuffer();

	void close();
};