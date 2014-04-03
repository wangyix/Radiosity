#include "SceneShaderInterface.h"


SceneShaderInterface::SceneShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	numVertices(0), numIndices(0),
	positionVbo(0), texcoordVbo(0), indexVbo(0),
	modelViewProj(-1), tex(-1) {
}


// returns true if success
void SceneShaderInterface::init() {
	
	glGetError();
	
	// create shaders

	vertShader = Utils::loadShaderFromFile("./shaders/scene.vert", GL_VERTEX_SHADER);
	fragShader = Utils::loadShaderFromFile("./shaders/scene.frag", GL_FRAGMENT_SHADER);

	// link shader objects
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
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
		
		exit(-1);
	}
	
	// get uniform locations
	modelViewProj = glGetUniformLocation(shaderProgram, "_modelViewProj");
	tex = glGetUniformLocation(shaderProgram, "_tex");

	glUseProgram(shaderProgram);

	// use texture unit 0 for tex
	glUniform1i(tex, 0);	

	Utils::exitOnGLError("ERROR: could not create shaders");
	

	// allocate vbos

	glGenBuffers(1, &positionVbo);
	glGenBuffers(1, &texcoordVbo);
	glGenBuffers(1, &indexVbo);


	Utils::exitOnGLError("ERROR: could not allocate vbos");
}





void SceneShaderInterface::setVertices(int numVertices, const float *positions, const float *texcoords,
		int numIndices, const unsigned short *indices) {

	// set up vbos

	GLenum ErrorCheckValue = glGetError();

	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertices*sizeof(float), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, 2*numVertices*sizeof(float), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(unsigned short), indices, GL_STATIC_DRAW);

	Utils::exitOnGLError("ERROR: could not set up vbos");
}





void SceneShaderInterface::setModelViewProj(const glm::mat4 &modelViewProj) {

	//glUseProgram(shaderProgram);
	
	glUniformMatrix4fv(this->modelViewProj, 1, GL_FALSE, glm::value_ptr(modelViewProj));
}



void SceneShaderInterface::setTexture(GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, 0);	// use texture's sampler state
}



void SceneShaderInterface::draw() {

	//glUseProgram(shaderProgram);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}




void SceneShaderInterface::close() {
	
	glGetError();
	
	// destroy vbos

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &texcoordVbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexVbo);

	Utils::exitOnGLError("ERROR: could not destroy vbos");


	// destroy shaders

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);

	Utils::exitOnGLError("ERROR: could not destroy shaders");
}