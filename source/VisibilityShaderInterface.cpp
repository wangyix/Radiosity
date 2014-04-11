#include "VisibilityShaderInterface.h"

VisibilityShaderInterface::VisibilityShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	tessControlShader(0), tessEvalShader(0),
	numVertices(0),
	vao(0), positionVbo(0), idVbo(0),
	modelView(-1), nearFar(-1),
	visFramebuffer(0), visTexture(0), visDepth(0) {
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
	
	
	
	/*
	
	// create framebuffer for use as render target
	glGenFramebuffers(1, &visFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, visFramebuffer);

	// create texture for rendering to
	glGenTextures(1, &visTexture);

	glBindTexture(GL_TEXTURE_2D, visTexture);

	// give it empty image initially	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, VIS_BUFFER_WIDTH,
			VIS_BUFFER_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);

	// set filtering to nearest: we don't want interpolation of ID values
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// border color should be 0 by default

	
	// create a depth buffer
	glGenRenderbuffers(1, &visDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, visDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
		VIS_BUFFER_WIDTH, VIS_BUFFER_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, visDepth);
	

	// attach to frame buffer object
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, visTexture, 0);


	// set list of draw buffers
	GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers);

	
	// check framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "ERROR: could not create framebuffer for visibility shader");
		printf("Press enter to exit...");
		getchar();
		exit(EXIT_FAILURE);
	}

	//Utils::exitOnGLError("ERROR: coud not create framebuffer");
	*/


	
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
	/*
	glBindFramebuffer(GL_FRAMEBUFFER, visFramebuffer);
	glViewport(0, 0, VIS_BUFFER_WIDTH, VIS_BUFFER_HEIGHT);
	*/

	glUseProgram(shaderProgram);

	glBindVertexArray(vao);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawArrays(GL_PATCHES, 0, numVertices);

	/*
	// clean up
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	
	// read back
	unsigned int *ids = new unsigned int[VIS_BUFFER_WIDTH*VIS_BUFFER_HEIGHT];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, ids);

	for (int i=200; i<201; i++) {
		for (int j=0; j<VIS_BUFFER_WIDTH; j++) {
			printf(" %d", ids[i*VIS_BUFFER_WIDTH+j]);
		}
		printf("\n");
	}
	delete[] ids;

	Utils::exitOnGLError("ERROR: end of draw()");
	getchar();
	exit(1);
	*/
}



void VisibilityShaderInterface::renderVisibilityBuffer() {


}


void VisibilityShaderInterface::close() {
	
	glGetError();
	
	// destroy vbos

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &idVbo);

	Utils::exitOnGLError("ERROR: could not destroy vsi vbos");


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