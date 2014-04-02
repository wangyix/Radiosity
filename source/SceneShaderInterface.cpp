#include "SceneShaderInterface.h"


SceneShaderInterface::SceneShaderInterface()
	: shaderProgram(0), numVertices(0), numIndices(0),
	vao(0), positionVbo(0), texcoordVbo(0), indexVbo(0),
	modelViewProj(0), tex(0) {

}


// returns true if success
int SceneShaderInterface::init() {

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
	glBindAttribLocation(shaderProgram, 0, "position");
	glBindAttribLocation(shaderProgram, 1, "texcoord");

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


	// get uniform parameters
	modelViewProj = glGetUniformLocation(shaderProgram, "_modelViewProj");
	tex = glGetUniformLocation(shaderProgram, "_tex");
	
	// use texture unit 0 for tex
	glUniform1i(tex, 0);	


	// create vertex array object
	glGenVertexArrays(1, &vao);
	
	// create vertex buffer objects
	glGenBuffers(1, &positionVbo);
	glGenBuffers(1, &texcoordVbo);
	glGenBuffers(1, &indexVbo);
	
	// set up vao...

	glBindVertexArray(vao);

	// associate vertex attribute 0 (position) with positionVbo
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// associate vertex attribute 1 (texcoord) with texcoordVbo
	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	// enable access for those attributes
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	return 0;
}



void SceneShaderInterface::setVertices(int numVertices, const float *positions, const float *texcoords,
	int numIndices, const unsigned short *indices) {

	// give data to position and texcoord vbos
	this->numVertices = numVertices;
	this->numIndices = numIndices;
	glBindBuffer(GL_ARRAY_BUFFER, positionVbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices*3*sizeof(float), positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordVbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices*2*sizeof(float), texcoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(unsigned short), indices,
		GL_STATIC_DRAW);
}


void SceneShaderInterface::bindProgram() {
	
	glUseProgram(shaderProgram);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
}


void SceneShaderInterface::setModelViewProj(const glm::mat4 &modelViewProj) {
	glUniformMatrix4fv(this->modelViewProj, 1, GL_FALSE, glm::value_ptr(modelViewProj));
}

void SceneShaderInterface::setTexture(GLuint texture) {
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glBindSampler(0, 0);	// use texture's sampler state
}


void SceneShaderInterface::draw() {
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
}


void SceneShaderInterface::close() {

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
}