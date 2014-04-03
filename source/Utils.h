#pragma once

#include "glincludes.h"
#include <fstream>

namespace Utils {

	void exitOnGLError(const char* error_message);
	GLuint loadShaderFromFile(char* path, GLenum shaderType);

}