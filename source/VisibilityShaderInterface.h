#pragma once

#include "glincludes.h"
#include "Utils.h"


class VisibilityShaderInterface {

private:
	GLuint shaderProgram;
	GLuint vertShader;
	GLuint tessControlShader;
	GLuint tessEvalShader;
	GLuint fragShader;

	int numVertices;
	GLuint positionVbo;
	GLuint quadIdVbo;

	GLint modelView;
	GLint nearFar;

public:
	
	VisibilityShaderInterface();
	
	void init();
	
	void setVertices(int numVertices, const float *positions, const int *ids);

	void setNearFar(float near, float far);
	void setModelView(const glm::mat4 &modelView);

	void draw();

	void close();
};