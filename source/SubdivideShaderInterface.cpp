#include "SubdivideShaderInterface.h"

SubdivideShaderInterface::SubdivideShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	radTex(-1), resTex(-1),
	vao(0), texcoordVbo(0), indexVbo(0),
	fbo(0), bilinearClampToEdgeSampler(0) {
}


void SubdivideShaderInterface::init() {

	glGetError();
	
	// load shaders
	vertShader = Utils::loadShaderFromFile("./shaders/subdivide.vert", GL_VERTEX_SHADER);
	fragShader = Utils::loadShaderFromFile("./shaders/subdivide.frag", GL_FRAGMENT_SHADER);

	// link shader objects
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	Utils::linkShaderProgram(shaderProgram);
	
	// get uniform locations
	radTex = glGetUniformLocation(shaderProgram, "_radTex");
	resTex = glGetUniformLocation(shaderProgram, "_resTex");

	Utils::exitOnGLError("ERROR: could not create susi shader program");

	
	// assign texture units to tex uniforms
	glUseProgram(shaderProgram);
	glUniform1i(radTex, 0);
	glUniform1i(resTex, 1);

	
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

	Utils::exitOnGLError("ERROR: could not set up susi vbos");


	
	// set up fbo

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// set list of draw buffers: color attachments 0 thru 7 are rendered to
	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
							GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
							GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
							GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
	glDrawBuffers(8, drawBuffers);

	/*
	// check framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "ERROR: could not create susi fbo \
						(failed to return GL_FRAMEBUFFER_COMPLETE)");
		printf("Press enter to exit...");
		getchar();
		exit(EXIT_FAILURE);
	}*/

	// unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Utils::exitOnGLError("ERROR: coud not set up susi fbo");

	
	// set up texture sampler object
	
	glGenSamplers(1, &bilinearClampToEdgeSampler);
	glSamplerParameteri(bilinearClampToEdgeSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(bilinearClampToEdgeSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(bilinearClampToEdgeSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(bilinearClampToEdgeSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	Utils::exitOnGLError("ERROR: coud not set up susi sampler object");
}



void SubdivideShaderInterface::setUniforms(GLuint radTex, GLuint resTex) {

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, radTex);
	glBindSampler(0, bilinearClampToEdgeSampler);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, resTex);
	glBindSampler(1, bilinearClampToEdgeSampler);
}




void SubdivideShaderInterface::draw(GLuint radTexBL, GLuint resTexBL,
									GLuint radTexBR, GLuint resTexBR,
									GLuint radTexTL, GLuint resTexTL,
									GLuint radTexTR, GLuint resTexTR,
									int texWidth, int texHeight) {

	// set framebuffer, viewport
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, texWidth, texHeight);

	// disable depth test and depth writes
	glDisable(GL_DEPTH_TEST);

	// attach rad and res textures of quadrants as color attachments to fbo
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, radTexBL, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, resTexBL, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, radTexBR, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, resTexBR, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, radTexTL, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, resTexTL, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, radTexTR, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, resTexTR, 0);
	
	// draw

	glUseProgram(shaderProgram);

	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);


	// unbind framebuffer, restore original viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, vp[2], vp[3]);

	// re-enable depth test and depth writes
	glEnable(GL_DEPTH_TEST);



	// rebuild mipmaps of textures we rendered to
	// TODO mipmap generation for a quadrant may not be necessary if further subdivision is required
	
	glBindTexture(GL_TEXTURE_2D, radTexBL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, resTexBL);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, radTexBR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, resTexBR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, radTexTL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, resTexTL);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, radTexTR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, resTexTR);
	glGenerateMipmap(GL_TEXTURE_2D);
}




void SubdivideShaderInterface::close() {

	glGetError();

	// destroy vbos
	
	glDeleteVertexArrays(1, &vao);

	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &texcoordVbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexVbo);

	Utils::exitOnGLError("ERROR: could not destroy susi vao, vbos");

	// destroy fbo

	glDeleteFramebuffers(1, &fbo);

	Utils::exitOnGLError("ERROR: could not destroy susi fbo");


	// destroy shaders

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glDeleteProgram(shaderProgram);

	Utils::exitOnGLError("ERROR: could not destroy susi shaders");


	// destroy sampler

	glDeleteSamplers(1, &bilinearClampToEdgeSampler);

	Utils::exitOnGLError("ERROR: could not destroy susi sampler object");
}