#pragma once

#include "glincludes.h"
#include "SceneShaderInterface.h"
#include "Quad.h"
#include "QuadMesh.h"
#include "Camera.h"

#include <stdio.h>

class Scene {
private:

	SceneShaderInterface *ssi;

	QuadMesh *quadMesh;

	Camera camera;

public:

	Scene();

	int init();

	void onResize(int w, int h);

	void update(double delta);
	void render();

	void close();
};