#include "SceneShaderInterface.h"


SceneShaderInterface::SceneShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	numVertices(0), numIndices(0),
	vao(0), positionVbo(0), texcoordVbo(0), indexVbo(0),
	modelViewProj(-1), tex(-1),
	nearestClampToEdgeSampler(0) {
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

	// assign texture units to tex uniforms
	glUseProgram(shaderProgram);
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

	// set up texture sampler object
	
	glGenSamplers(1, &nearestClampToEdgeSampler);
	glSamplerParameteri(nearestClampToEdgeSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestClampToEdgeSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestClampToEdgeSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(nearestClampToEdgeSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	Utils::exitOnGLError("ERROR: coud not set up ssi sampler object");
}



void SceneShaderInterface::setVertices(int numVertices, const float *positions,const float *texcoords,
		int numIndices, const unsigned short *indices) {

	this->numVertices = numVertices;
	this->numIndices = numIndices;

	// update vbos

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertices*sizeof(float), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, 2*numVertices*sizeof(float), texcoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(unsigned short), indices, GL_STATIC_DRAW);
}


void SceneShaderInterface::setModelViewProj(const glm::mat4 &modelViewProj) {

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(this->modelViewProj, 1, GL_FALSE, glm::value_ptr(modelViewProj));
}



void SceneShaderInterface::setTexture(GLuint texture) {

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, nearestClampToEdgeSampler);
}



void SceneShaderInterface::draw(int baseVertex, int windowWidth, int windowHeight) {

	// unbind any framebuffers, set viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	// enable depth test and depth writes
	glEnable(GL_DEPTH_TEST);

	// draw
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glDrawElementsBaseVertex(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL, baseVertex);
}




void SceneShaderInterface::close() {
	
	glGetError();
	
	// destroy vao, vbos

	glDeleteVertexArrays(1, &vao);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &texcoordVbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexVbo);

	Utils::exitOnGLError("ERROR: could not destroy ssi vao, vbos");


	// destroy shaders

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);

	Utils::exitOnGLError("ERROR: could not destroy ssi shaders");


	// destroy sampler

	glDeleteSamplers(1, &nearestClampToEdgeSampler);

	Utils::exitOnGLError("ERROR: could not destroy ssi sampler object");
}