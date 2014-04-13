#include "VisibilityShaderInterface.h"

VisibilityShaderInterface::VisibilityShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	tessControlShader(0), tessEvalShader(0),
	numVertices(0),
	vao(0), positionVbo(0), idVbo(0),
	modelView(-1), nearFar(-1),
	fbo(0), visTexture(0), visDepth(0) {
}

void VisibilityShaderInterface::init(int numVertices, const float *positions, const unsigned int *ids) {

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

	Utils::exitOnGLError("ERROR: could not create vsi shader program");
	
	
	this->numVertices = numVertices;

	// set up vao, vbos
		
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &positionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertices*sizeof(float), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &idVbo);
	glBindBuffer(GL_ARRAY_BUFFER, idVbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(unsigned int), ids, GL_STATIC_DRAW);
	//glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, 0, 0);	// doesn't work for ints for some reason
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 0, 0);
	glEnableVertexAttribArray(1);

	Utils::exitOnGLError("ERROR: could not set up vsi vbos");



	// set up fbo
	
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// create texture for rendering to, give it empty image initially
	glGenTextures(1, &visTexture);
	glBindTexture(GL_TEXTURE_2D, visTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, VIS_BUFFER_WIDTH,
			VIS_BUFFER_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	// set filtering to nearest: we don't want interpolation of ID values
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);	// border color should be 0 by default
	// attach texture as color attachment 0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, visTexture, 0);

	// create a depth buffer, attach to framebuffer
	glGenRenderbuffers(1, &visDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, visDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
		VIS_BUFFER_WIDTH, VIS_BUFFER_HEIGHT);
	// attach depth as depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, visDepth);
	
	// set list of draw buffers: only the visibility texture (color attachment 0) is rendered to
	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	// check framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "ERROR: could not create vsi fbo \
						(failed to return GL_FRAMEBUFFER_COMPLETE)");
		printf("Press enter to exit...");
		getchar();
		exit(EXIT_FAILURE);
	}

	// unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	Utils::exitOnGLError("ERROR: coud not set up vsi fbo");
}





void VisibilityShaderInterface::setNearFar(float near, float far) {

	glUseProgram(shaderProgram);

	glUniform2f(this->nearFar, near, far);
}





void VisibilityShaderInterface::setModelView(const glm::mat4 &modelView) {

	glUseProgram(shaderProgram);

	glUniformMatrix4fv(this->modelView, 1, GL_FALSE, glm::value_ptr(modelView));
}


void VisibilityShaderInterface::draw() {
	
	// set framebuffer, viewport
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, VIS_BUFFER_WIDTH, VIS_BUFFER_HEIGHT);

	
	// clear color attachment 0 and depth of framebuffer
	GLint clearColor[] = {0, 0, 0, 0};
	glClearBufferiv(GL_COLOR, 0, clearColor);
	GLfloat clearDepth[] = {1.0f};
	glClearBufferfv(GL_DEPTH, 0, clearDepth);


	// draw patch IDs

	glUseProgram(shaderProgram);

	glBindVertexArray(vao);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, numVertices);

	
	// unbind framebuffer, restore original viewport
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, vp[2], vp[3]);
	
	
	/*
	// TEST!!! read back
	glBindTexture(GL_TEXTURE_2D, visTexture);
	unsigned int *ids = new unsigned int[VIS_BUFFER_WIDTH*VIS_BUFFER_HEIGHT];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, ids);
	bool nonZero = false;
	for (int i=0; i<VIS_BUFFER_HEIGHT; i++) {
		for (int j=0; j<VIS_BUFFER_WIDTH; j++) {
			
			printf(" %d", ids[i*VIS_BUFFER_WIDTH+j]);
			if (ids[i*VIS_BUFFER_WIDTH+j] != 0) {
				nonZero = true;
			}
		}
		printf("\n");
	}
	delete[] ids;
	/*
	if (nonZero)
		printf("1");
	else
		printf("0");
	*/
	//getchar();
	
}


void VisibilityShaderInterface::close() {
	
	glGetError();
	
	// destroy vao, vbos

	glDeleteVertexArrays(1, &vao);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &idVbo);

	Utils::exitOnGLError("ERROR: could not destroy vsi vao, vbos");

	// destroy fbo

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &visTexture);
	glDeleteRenderbuffers(1, &visDepth);

	Utils::exitOnGLError("ERROR: could not destroy vsi fbo");

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

	Utils::exitOnGLError("ERROR: could not destroy vsi shaders");
}



GLuint VisibilityShaderInterface::getVisTexture() const {
	return visTexture;
}