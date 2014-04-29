#pragma once

#include "glincludes.h"
#include "SceneShaderInterface.h"
#include "VisibilityShaderInterface.h"
#include "ReconstructShaderInterface.h"
#include "SubdivideShaderInterface.h"
#include "QuadMesh.h"
#include "Camera.h"

#include <stdio.h>


#define SCENE_FILE "./scenefiles/cornell_box_notess.txt"

#define SHOOTER_LEVEL 3
#define MAX_SUBDIVIDE_LEVEL 2

#define CAMERA_MOVE_SPEED 3.0f		// dist per sec
#define CAMERA_ROTATE_SPEED 0.2f	// deg per pixel

class Scene {
private:

	SceneShaderInterface ssi;
	VisibilityShaderInterface vsi;
	ReconstructShaderInterface rsi;
	SubdivideShaderInterface susi;

	QuadMesh quadMesh;
	
	Quad *shooter;

	Camera camera;

	bool testKeyDown;
	bool testFlag;

public:

	Scene();

	int init();

	void onResize(int w, int h);

	void update(GLFWwindow *window, double delta);
	void render();

	void close();
};