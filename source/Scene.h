#pragma once

#include "glincludes.h"
#include "SceneShaderInterface.h"
#include "Quad.h"

#include <stdio.h>

class Scene {
private:

	SceneShaderInterface *ssi;


public:

	Scene();

	int init();

	void update(double delta);
	void render();

	void close();
};