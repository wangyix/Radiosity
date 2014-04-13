#include "ReconstructShaderInterface.h"

ReconstructShaderInterface::ReconstructShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	modelView(-1), id(-1), reflectance(-1), shooterPower(-1),
	normal(-1), visTex(-1), radTex(-1), resTex(-1),
	numVertices(0), numIndices(0),
	vao(0), positionVbo(0), texcoordVbo(0), indexVbo(0),
	fbo(0) {
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

	// unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// check framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "ERROR: could not create rsi fbo \
						(failed to return GL_FRAMEBUFFER_COMPLETE)");
		printf("Press enter to exit...");
		getchar();
		exit(EXIT_FAILURE);
	}

	// unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Utils::exitOnGLError("ERROR: coud not set up rsi fbo");
}



void ReconstructShaderInterface::setShooterUniforms(const glm::mat4 &modelView,
		const glm::vec3 &shooterPower, GLuint visibilityTexture) {

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(this->modelView, 1, GL_FALSE, glm::value_ptr(modelView));
	glUniform3fv(this->shooterPower, 1, glm::value_ptr(shooterPower));

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, visibilityTexture);
}


void ReconstructShaderInterface::setReceiverUniforms(unsigned int id,
		const glm::vec3 &reflectance, const glm::vec3 &normalShooterView,
		GLuint radTex, GLuint resTex) {

	glUseProgram(shaderProgram);

	glUniform1ui(this->id, id);
	glUniform3fv(this->reflectance, 1, glm::value_ptr(reflectance));
	glUniform3fv(this->normal, 1, glm::value_ptr(normalShooterView));

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, radTex);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, resTex);
}



void ReconstructShaderInterface::draw(int baseVertex,
		GLuint nextRadTex, GLuint nextResTex, int texWidth, int texHeight) {

	// set framebuffer, viewport
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, texWidth, texHeight);

	// disable depth test and depth writes
	glDisable(GL_DEPTH_TEST);

	// attach rad and res textures as color attachments to fbo
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, nextRadTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, nextResTex, 0);

	// draw

	glUseProgram(shaderProgram);

	glBindVertexArray(vao);

	glDrawElementsBaseVertex(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, NULL, baseVertex);
	

	// unbind framebuffer, restore original viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, vp[2], vp[3]);

	// re-enable depth test and depth writes
	glEnable(GL_DEPTH_TEST);



	// rebuild mipmaps of textures we rendered to

	glBindTexture(GL_TEXTURE_2D, nextRadTex);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, nextResTex);
	glGenerateMipmap(GL_TEXTURE_2D);

	/*
	// TEST!!! read back new residual texture
	printf("\n\n\n\n");
	glBindTexture(GL_TEXTURE_2D, nextResTex);
	float *irr = new float [3*texWidth*texHeight];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, irr);
	for (int i=0; i<texHeight; i++) {
		for (int j=0; j<texWidth; j++) {
			int base = 3*(i*texWidth+j);
			printf(" (%3.3f %3.3f %3.3f)", irr[base], irr[base+1], irr[base+2]);
		}
		printf("\n");
	}
	delete[] irr;
	*/
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
}