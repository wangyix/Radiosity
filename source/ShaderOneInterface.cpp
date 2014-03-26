#include "ShaderOneInterface.h"


ShaderOneInterface::ShaderOneInterface() {
	shaderProgram = 0;
}

// returns true if success
int ShaderOneInterface::init() {

	if (shaderProgram)
		return true;

	// read shader source text files and compile them into shader objects
	GLuint vertShader = ShaderLoader::
		loadShaderFromFile("./shaders/radiosity.vert", GL_VERTEX_SHADER);
	if (!vertShader) {
		fprintf(stderr, "ERROR: Compilation errors in vertex shader.\n");
		return 1;
	}

	GLuint fragShader = ShaderLoader::
		loadShaderFromFile("./shaders/radiosity.frag", GL_FRAGMENT_SHADER);
	if (!fragShader) {
		fprintf(stderr, "ERROR: Compilation errors in fragment shader.\n");
		return 2;
	}
	
	// link shader objects
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	// assign indices to vertex attributes
	glBindAttribLocation(shaderProgram, 0, "position");
	glBindAttribLocation(shaderProgram, 1, "normal");
	glBindAttribLocation(shaderProgram, 2, "texcoord");

	glLinkProgram(shaderProgram);

	// check for link errors
	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		
		GLint infoLen = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLen);
		char *infoLog = new char[infoLen];
		glGetProgramInfoLog(shaderProgram, infoLen, NULL, infoLog);
		fprintf(stderr, "Error linking shaders:\n%s\n", infoLog);
		delete infoLog;

		// clean up
		glDeleteProgram(shaderProgram);
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);
		
		return 3;
	}

	// link cleanup: detach shaders from program
	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);


	// get uniform parameters
	modelView = glGetUniformLocation(shaderProgram, "_mv");
	modelViewInvTrans = glGetUniformLocation(shaderProgram, "_mvInvTrans");
	modelViewProj = glGetUniformLocation(shaderProgram, "_mvProj");

	return 0;
}


void ShaderOneInterface::close() {
	glDeleteProgram(shaderProgram);
	shaderProgram = 0;
}