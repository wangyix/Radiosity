#pragma once

#include "glincludes.h"
#include <fstream>
//#include <iostream>
#include <string>

namespace Utils {

	void exitOnGLError(const char* error_message);
	
	GLuint loadShaderFromFile(char* path, GLenum shaderType);
	GLint linkShaderProgram(GLuint program);

	std::string trim(const std::string &str, const std::string &whitespace=" \t");
}