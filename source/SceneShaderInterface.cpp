#include "SceneShaderInterface.h"


SceneShaderInterface::SceneShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	numVertices(0), numIndices(0),
	vao(0), positionVbo(0), texcoordVbo(0), indexVbo(0),
	modelViewProj(-1), tex(-1) {
}


void SceneShaderInterface::init(int numVertices, const float *positions, const float *texcoords,
		int numIndices, const unsigned short *indices) {
	
	glGetError();
	
	// load shaders
	vertShader = Utils::loadShaderFromFile("./shaders/scene.vert", GL_VERTEX_SHADER);
	fragShader = Utils::loadShaderFromFile("./shaders/scene.frag", GL_FRAGMENT_SHADER);

	// link shader objects
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	Utils::linkShaderProgram(shaderProgram);
	
	// get uniform locations
	modelViewProj = glGetUniformLocation(shaderProgram, "_modelViewProj");
	tex = glGetUniformLocation(shaderProgram, "_tex");

	glUseProgram(shaderProgram);

	// use texture unit 0 for tex
	glUniform1i(tex, 0);	

	Utils::exitOnGLError("ERROR: could not create ssi shader program");
	


	this->numVertices = numVertices;
	this->numIndices = numIndices;

	// set up vao, vbos
		
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &positionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertices*sizeof(float), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &texcoordVbo);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, 2*numVertices*sizeof(float), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &indexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(unsigned short), indices, GL_STATIC_DRAW);

	Utils::exitOnGLError("ERROR: could not set up ssi vbos");
}




void SceneShaderInterface::setModelViewProj(const glm::mat4 &modelViewProj) {

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(this->modelViewProj, 1, GL_FALSE, glm::value_ptr(modelViewProj));
}



void SceneShaderInterface::setTexture(GLuint texture) {

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, 0);	// use texture's sampler state
}



void SceneShaderInterface::draw(int baseVertex) {

	glUseProgram(shaderProgram);

	glBindVertexArray(vao);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);

	glDrawElementsBaseVertex(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL, baseVertex);
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

	Utils::exitOnGLError("ERROR: could not destroy ssi vbos");


	// destroy shaders

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);

	Utils::exitOnGLError("ERROR: could not destroy ssi shaders");
}