
#include "Utils.h"


void Utils::exitOnGLError(const char* error_message) {

	const GLenum ErrorValue = glGetError();
	if (ErrorValue != GL_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
		printf("Press enter to exit...");
		getchar();
		exit(EXIT_FAILURE);
	}
}


GLuint Utils::loadShaderFromFile(char* path, GLenum shaderType) {

	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile(path);

	if (sourceFile) {
		
		//Get shader source
		shaderString.assign(std::istreambuf_iterator<char>(sourceFile),
			std::istreambuf_iterator<char>() );
		
		//Create shader ID
		shaderID = glCreateShader(shaderType);

		//Set shader source
		const GLchar* shaderSource = shaderString.c_str();
		glShaderSource(shaderID, 1, &shaderSource, NULL);

		//Compile shader source
		glCompileShader(shaderID);

		//Check shader for errors
		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderCompiled);
		if(shaderCompiled != GL_TRUE) {
			GLint infoLen = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);

			char* infoLog = new char[infoLen];
			glGetShaderInfoLog(shaderID, infoLen, NULL, infoLog);
			fprintf(stderr, "Error compiling shader:\n%s\n", infoLog);
			delete infoLog;

			glDeleteShader(shaderID);
			return 0;
		}
	} else {
		printf("Unable to open file %s\n", path);
	}
	return shaderID;
}


GLint Utils::linkShaderProgram(GLuint program) {

	glLinkProgram(program);

	// check for link errors
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		char *infoLog = new char[infoLen];
		glGetProgramInfoLog(program, infoLen, NULL, infoLog);
		fprintf(stderr, "Error linking shaders:\n%s\n", infoLog);
		delete infoLog;

		// clean up
		int numAttachedShaders;
		GLuint attachedShaders[20];
		glGetAttachedShaders(program, 20, &numAttachedShaders, attachedShaders);

		glDeleteProgram(program);
		for (int i=0; i<numAttachedShaders; i++) {
			glDeleteShader(attachedShaders[i]);
		}
	}
	return isLinked;
}


std::string Utils::trim(const std::string &str, const std::string &whitespace) {
	const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}