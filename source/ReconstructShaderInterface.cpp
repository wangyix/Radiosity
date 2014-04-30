#include "ReconstructShaderInterface.h"

ReconstructShaderInterface::ReconstructShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	modelView(-1), id(-1), reflectance(-1), visTexelSize(-1), shooterPower(-1),
	normal(-1), visTex(-1), radTex(-1), resTex(-1),
	numVertices(0), numIndices(0),
	vao(0), positionVbo(0), texcoordVbo(0), indexVbo(0),
	fbo(0), nearestSampler(0), nearestClampToBorderSampler(0) {
}


void ReconstructShaderInterface::init(int numVertices, const float *positions, const float *texcoords,
		int numIndices, const unsigned short *indices) {

	glGetError();
	
	// load shaders
	vertShader = Utils::loadShaderFromFile("./shaders/reconstruct.vert", GL_VERTEX_SHADER);
	fragShader = Utils::loadShaderFromFile("./shaders/reconstruct.frag", GL_FRAGMENT_SHADER);

	// link shader objects
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	Utils::linkShaderProgram(shaderProgram);
	
	// get uniform locations
	modelView = glGetUniformLocation(shaderProgram, "_modelView");
	id = glGetUniformLocation(shaderProgram, "_id");
	reflectance = glGetUniformLocation(shaderProgram, "_reflectance");
	visTexelSize = glGetUniformLocation(shaderProgram, "_visTexelSize");
	shooterPower = glGetUniformLocation(shaderProgram, "_shooterPower");
	normal = glGetUniformLocation(shaderProgram, "_normal");
	visTex = glGetUniformLocation(shaderProgram, "_visTex");
	radTex = glGetUniformLocation(shaderProgram, "_radTex");
	resTex = glGetUniformLocation(shaderProgram, "_resTex");

	Utils::exitOnGLError("ERROR: could not create rsi shader program");


	// assign texture units to tex uniforms
	glUseProgram(shaderProgram);
	glUniform1i(visTex, 0);
	glUniform1i(radTex, 1);
	glUniform1i(resTex, 2);


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

	Utils::exitOnGLError("ERROR: could not set up rsi vbos");


	// set up fbo

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// set list of draw buffers: color attachments 0 and 1 (new rad and res) are rendered to
	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, drawBuffers);

	Utils::exitOnGLError("ERROR: coud not set up rsi fbo");


	// set up texture sampler object
	
	glGenSamplers(1, &nearestSampler);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenSamplers(1, &nearestClampToBorderSampler);
	glSamplerParameteri(nearestClampToBorderSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestClampToBorderSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(nearestClampToBorderSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(nearestClampToBorderSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	unsigned int borderColor[] = {0, 0, 0, 0};
	glSamplerParameterIuiv(nearestClampToBorderSampler, GL_TEXTURE_BORDER_COLOR, borderColor);

	Utils::exitOnGLError("ERROR: coud not set up rsi sampler objects");
}


void ReconstructShaderInterface::setVisTexelSize(float du, float dv) {
	
	glUseProgram(shaderProgram);

	glUniform2f(visTexelSize, du, dv);

}


void ReconstructShaderInterface::setShooterUniforms(const glm::vec3 &shooterPower) {

	glUseProgram(shaderProgram);

	glUniform3fv(this->shooterPower, 1, glm::value_ptr(shooterPower));

}


void ReconstructShaderInterface::setReceiverUniforms(const glm::mat4 &modelView,
		unsigned int id, const glm::vec3 &reflectance,
		const glm::vec3 &normalShooterView) {

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(this->modelView, 1, GL_FALSE, glm::value_ptr(modelView));
	glUniform1ui(this->id, id);
	glUniform3fv(this->reflectance, 1, glm::value_ptr(reflectance));
	glUniform3fv(this->normal, 1, glm::value_ptr(normalShooterView));

	
}


void ReconstructShaderInterface::setTextureUniforms(GLuint visibilityTex,
			GLuint radiosityTex, GLuint residualTex) {

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, visibilityTex);
	glBindSampler(0, nearestClampToBorderSampler);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, radiosityTex);
	glBindSampler(1, nearestSampler);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, residualTex);
	glBindSampler(2, nearestSampler);
}


void ReconstructShaderInterface::draw(GLuint nextRadTex, GLuint nextResTex,
		int texWidth, int texHeight) {

	// set framebuffer, viewport
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, texWidth, texHeight);
	// disable depth test and depth writes
	glDisable(GL_DEPTH_TEST);

	// attach rad and res textures as color attachments to fbo
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, nextRadTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, nextResTex, 0);

	/*
	// check framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "ERROR: could not create rsi fbo \
						(failed to return GL_FRAMEBUFFER_COMPLETE)");
		printf("Press enter to exit...");
		getchar();
		exit(EXIT_FAILURE);
	}*/

	// draw
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

	// rebuild mipmaps of textures we rendered to
	glBindTexture(GL_TEXTURE_2D, nextRadTex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, nextResTex);
	glGenerateMipmap(GL_TEXTURE_2D);
}


void ReconstructShaderInterface::close() {

	glGetError();

	// destroy vbos
	
	glDeleteVertexArrays(1, &vao);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &texcoordVbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexVbo);

	Utils::exitOnGLError("ERROR: could not destroy rsi vao, vbos");

	// destroy fbo

	glDeleteFramebuffers(1, &fbo);

	Utils::exitOnGLError("ERROR: could not destroy rsi fbo");


	// destroy shaders

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);

	Utils::exitOnGLError("ERROR: could not destroy rsi shaders");


	// destroy sampler

	glDeleteSamplers(1, &nearestSampler);
	glDeleteSamplers(1, &nearestClampToBorderSampler);

	Utils::exitOnGLError("ERROR: could not destroy rsi sampler object");
}