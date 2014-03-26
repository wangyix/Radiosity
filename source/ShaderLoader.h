#pragma once

#include "glincludes.h"
#include <fstream>

namespace ShaderLoader {
	GLuint loadShaderFromFile(char* path, GLenum shaderType);
}