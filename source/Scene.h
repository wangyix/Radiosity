#pragma once

#include "glincludes.h"
#include "ShaderOneInterface.h"

#include <stdio.h>

class Scene {
private:

	ShaderOneInterface *soi;

public:

	Scene();

	int init();

	void update(double delta);
	void render();

	void close();
};