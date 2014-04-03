#include "SceneShaderInterface.h"


SceneShaderInterface::SceneShaderInterface()
	: shaderProgram(0), vertShader(0), fragShader(0),
	numVertices(0), numIndices(0),
	vao(0), positionVbo(0), texcoordVbo(0), indexVbo(0),
	modelViewProj(-1), tex(-1) {

}


// returns true if success
int SceneShaderInterface::init() {
	/*
	// read shader source text files and compile them into shader objects
	GLuint vertShader = ShaderLoader::
		loadShaderFromFile("./shaders/scene.vert", GL_VERTEX_SHADER);
	if (!vertShader) {
		fprintf(stderr, "ERROR: Compilation errors in vertex shader.\n");
		return 1;
	}

	GLuint fragShader = ShaderLoader::
		loadShaderFromFile("./shaders/scene.frag", GL_FRAGMENT_SHADER);
	if (!fragShader) {
		fprintf(stderr, "ERROR: Compilation errors in fragment shader.\n");
		return 2;
	}
	
	// link shader objects
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	// assign indices to vertex attributes
	//glBindAttribLocation(shaderProgram, 0, "position");
	//glBindAttribLocation(shaderProgram, 1, "texcoord");

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


	glUseProgram(shaderProgram);


	GLint positionAttribLocation = glGetAttribLocation(shaderProgram, "position");
	GLint texcoordAttribLocation = glGetAttribLocation(shaderProgram, "texcoord");


	// get uniform parameters
	modelViewProj = glGetUniformLocation(shaderProgram, "_modelViewProj");
	tex = glGetUniformLocation(shaderProgram, "_tex");
	


	// use texture unit 0 for tex
	// glUniform1i(tex, 0);	

	// create vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// create vertex buffer objects
	
	glGenBuffers(1, &positionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), QuadMesh::positions, GL_STATIC_DRAW);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(positionAttribLocation);

	glGenBuffers(1, &texcoordVbo);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), QuadMesh::texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(texcoordAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(texcoordAttribLocation);

	glGenBuffers(1, &indexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned short), QuadMesh::indices, GL_STATIC_DRAW);

	
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		fprintf(stderr, "ERROR in init(): %s\n", gluErrorString(error));
	}

	return 0;
	*/

	// create shaders

	GLenum ErrorCheckValue = glGetError();

	vertShader = ShaderLoader::loadShaderFromFile("./shaders/scene.vert", GL_VERTEX_SHADER);
	fragShader = ShaderLoader::loadShaderFromFile("./shaders/scene.frag", GL_FRAGMENT_SHADER);

	// link shader objects
	GLuint shaderProgram = glCreateProgram();
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




	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		fprintf(stderr, "ERROR: Could not create the shaders: %s \n", gluErrorString(ErrorCheckValue));
		exit(-1);
	}
	


	// create vbos

	ErrorCheckValue = glGetError();

	glGenBuffers(1, &positionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), QuadMesh::positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &texcoordVbo);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), QuadMesh::texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &indexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned short), QuadMesh::indices, GL_STATIC_DRAW);


	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
		exit(-1);
	}
}



void SceneShaderInterface::setModelViewProj(const glm::mat4 &modelViewProj) {

	//glUseProgram(shaderProgram);
	
	glUniformMatrix4fv(this->modelViewProj, 1, GL_FALSE, glm::value_ptr(modelViewProj));
}


void SceneShaderInterface::draw() {

	//glUseProgram(shaderProgram);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
}




void SceneShaderInterface::close() {
	/*
	glBindVertexArray(vao);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &texcoordVbo);
	numVertices = 0;
	numIndices = 0;

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);

	glDeleteProgram(shaderProgram);
	shaderProgram = 0;
	*/

	// destroy shaders
	GLenum ErrorCheckValue = glGetError();

	glUseProgram(0);

	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	glDeleteShader(fragShader);
	glDeleteShader(vertShader);

	glDeleteProgram(shaderProgram);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		fprintf(stderr, "ERROR: Could not destroy the shaders: %s \n", gluErrorString(ErrorCheckValue));
		exit(-1);
	}


	// destroy vbos

	ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &positionVbo);
	glDeleteBuffers(1, &texcoordVbo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexVbo);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR) {
		fprintf(stderr, "ERROR: Could not destroy the VBO: %s \n", gluErrorString(ErrorCheckValue));
		exit(-1);
	}
}










void SceneShaderInterface::setVertices(int numVertices, const float *positions, const float *texcoords,
int numIndices, const unsigned short *indices) {

	// set up vao...

	glBindVertexArray(vao);

	// associate vertex attribute 0 (position) with positionVbo
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices*3*sizeof(float), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// associate vertex attribute 1 (texcoord) with texcoordVbo
	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices*2*sizeof(float), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);


	// enable access for those attributes

	

	// give data to position and texcoord vbos
	this->numVertices = numVertices;
	this->numIndices = numIndices;

	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	//glBufferData(GL_ARRAY_BUFFER, numVertices*3*sizeof(float), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	//glBufferData(GL_ARRAY_BUFFER, numVertices*2*sizeof(float), texcoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(unsigned short), indices, GL_STATIC_DRAW);
	
}


void SceneShaderInterface::bindProgram() {
	
	glUseProgram(shaderProgram);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
}



void SceneShaderInterface::setTexture(GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glBindSampler(0, 0);	// use texture's sampler state
}
