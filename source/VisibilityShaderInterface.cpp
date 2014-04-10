#include "VisibilityShaderInterface.h"

VisibilityShaderInterface::VisibilityShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	tessControlShader(0), tessEvalShader(0),
	numVertices(0),
	positionVbo(0), quadIdVbo(0),
	modelView(-1), nearFar(-1) {
}

void VisibilityShaderInterface::init() {

	glGetError();
	
	// load shaders
	vertShader = Utils::loadShaderFromFile("./shaders/visibility.vert", GL_VERTEX_SHADER);
	tessControlShader = Utils::loadShaderFromFile("./shaders/visibility.tc.glsl", GL_TESS_CONTROL_SHADER);
	tessEvalShader = Utils::loadShaderFromFile("./shaders/visibility.te.glsl", GL_TESS_EVALUATION_SHADER);
	fragShader = Utils::loadShaderFromFile("./shaders/visibility.frag", GL_FRAGMENT_SHADER);

	// link shader objects
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, tessControlShader);
	glAttachShader(shaderProgram, tessEvalShader);
	glAttachShader(shaderProgram, fragShader);
	Utils::linkShaderProgram(shaderProgram);
	
	// get uniform locations
	modelView = glGetUniformLocation(shaderProgram, "_modelView");
	nearFar = glGetUniformLocation(shaderProgram, "_nearFar");

	glUseProgram(shaderProgram);	

	Utils::exitOnGLError("ERROR: could not create shader program");
	
	
	// allocate vbos

	glGenBuffers(1, &positionVbo);
	glGenBuffers(1, &quadIdVbo);

	Utils::exitOnGLError("ERROR: could not allocate vbos");
}



void VisibilityShaderInterface::setVertices(int numVertices,
	const float *positions, const int *ids) {

	this->numVertices = numVertices;

	// set up vbos

	GLenum ErrorCheckValue = glGetError();

	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertices*sizeof(float), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, quadIdVbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(int), ids, GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 1, GL_INT, GL_FALSE, 0, 0);	// doesn't work for ints for some reason
	glVertexAttribIPointer(1, 1, GL_INT, 0, 0);
	glEnableVertexAttribArray(1);

	Utils::exitOnGLError("ERROR: could not set up vbos");
}


void VisibilityShaderInterface::setNearFar(float near, float far) {
	glUniform2f(this->nearFar, near, far);
}

void VisibilityShaderInterface::setModelView(const glm::mat4 &modelView) {
	glUniformMatrix4fv(this->modelView, 1, GL_FALSE, glm::value_ptr(modelView));
}


void VisibilityShaderInterface::draw() {
	//glDrawElements(, 6, GL_UNSIGNED_SHORT, NULL);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, numVertices);
}



void VisibilityShaderInterface::close() {
	
	glGetError();
	
	// destroy vbos

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &quadIdVbo);

	Utils::exitOnGLError("ERROR: could not destroy vbos");


	// destroy shaders

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, tessControlShader);
	glDetachShader(shaderProgram, tessEvalShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(tessControlShader);
	glDeleteShader(tessEvalShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);

	Utils::exitOnGLError("ERROR: could not destroy shaders");
}