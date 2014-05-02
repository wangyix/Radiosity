#pragma once

#include "glincludes.h"
#include "SceneShaderInterface.h"
#include "VisibilityShaderInterface.h"
#include "Quad.h"
#include "Camera.h"

#include <stdio.h>


#define CAMERA_MOVE_SPEED 3.0f		// dist per sec
#define CAMERA_ROTATE_SPEED 0.2f	// deg per pixel

class Scene {
private:

	SceneShaderInterface *ssi;
	VisibilityShaderInterface *vsi;

	int numQuads;
	Quad *quads;

	float *quadPositions;
	unsigned int *quadIds;

	Camera camera;

public:

	Scene();

	int init();

	void onResize(int w, int h);

	void update(GLFWwindow *window, double delta);
	void render();

	void close();
};