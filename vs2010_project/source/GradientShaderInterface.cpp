#include "GradientShaderInterface.h"

GradientShaderInterface::GradientShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	radTexelSize(-1), threshold(-1), radTex(-1),
	vao(0), texcoordVbo(0), indexVbo(0),
	nearestSampler(0) {
}


void GradientShaderInterface::init() {

	glGetError();
	
	// load shaders
	vertShader = Utils::loadShaderFromFile("./shaders/gradient.vert", GL_VERTEX_SHADER);
	fragShader = Utils::loadShaderFromFile("./shaders/gradient.frag", GL_FRAGMENT_SHADER);

	// link shader objects
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	Utils::linkShaderProgram(shaderProgram);
	
	// get uniform locations
	radTexelSize = glGetUniformLocation(shaderProgram, "_radTexelSize");
	threshold = glGetUniformLocation(shaderProgram, "_threshold");
	radTex = glGetUniformLocation(shaderProgram, "_radTex");

	Utils::exitOnGLError("ERROR: could not create gsi shader program");


	// assign texture units to tex uniforms
	glUseProgram(shaderProgram);
	glUniform1i(radTex, 0);



	// set up vao, vbos

	// vertex data for drawing screen quad
	float texcoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
	unsigned short indices[] = {0, 1, 2, 0, 2, 3};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &texcoordVbo);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &indexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned short), indices, GL_STATIC_DRAW);

	Utils::exitOnGLError("ERROR: could not set up gsi vbos");

	// set up query object

	glGenQueries(1, &query);

	Utils::exitOnGLError("ERROR: could not set up gsi query object");

	// set up texture sampler object
	
	glGenSamplers(1, &nearestSampler);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	Utils::exitOnGLError("ERROR: coud not set up gsi sampler object");
}



void GradientShaderInterface::setThresholdAndRadTexelSize(
		float threshold, float du, float dv) {

	glUseProgram(shaderProgram);

	glUniform1f(this->threshold, threshold);
	glUniform2f(radTexelSize, du, dv);
}



void GradientShaderInterface::setTexture(GLuint radTex)  {

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, radTex);
	glBindSampler(0, nearestSampler);
}



int GradientShaderInterface::draw(int texWidth, int texHeight) {

	// detach any framebuffer, set viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, texWidth, texHeight);

	// disable depth test and depth writes
	glDisable(GL_DEPTH_TEST);

	// disable writes to color
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


	// begin hardware occlusion query
	glBeginQuery(GL_SAMPLES_PASSED, query);

	// draw

	glUseProgram(shaderProgram);

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	// end hoq
	glEndQuery(GL_SAMPLES_PASSED);


	// wait for and retrieve query results
	GLint resultAvailable;
	do {
		glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &resultAvailable);
	} while (!resultAvailable);
	GLuint samplesPassed;
	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &samplesPassed);


	// re-enable color writes
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


	// return number of pixels discarded
	return texWidth*texHeight - (int)samplesPassed;
}



void GradientShaderInterface::close() {

	glGetError();

	// destroy vbos
	
	glDeleteVertexArrays(1, &vao);

	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &texcoordVbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexVbo);

	Utils::exitOnGLError("ERROR: could not destroy gsi vao, vbos");


	// destroy shaders

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);

	Utils::exitOnGLError("ERROR: could not destroy gsi shaders");


	// destroy query object

	glDeleteQueries(1, &query);

	Utils::exitOnGLError("ERROR: could not destroy gsi query object");


	// destroy sampler

	glDeleteSamplers(1, &nearestSampler);

	Utils::exitOnGLError("ERROR: could not destroy gsi sampler object");
}