#pragma once

#include "glincludes.h"
#include "SceneShaderInterface.h"
#include "Quad.h"
#include "Camera.h"

#include <stdio.h>

class Scene {
private:

	SceneShaderInterface *ssi;

	int numQuads;
	Quad *quads;

	Camera camera;

public:

	Scene();

	int init();

	void onResize(int w, int h);

	void update(double delta);
	void render();

	void close();
};