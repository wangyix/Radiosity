
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