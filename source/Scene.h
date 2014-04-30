#pragma once

#include "glincludes.h"
#include "SceneShaderInterface.h"
#include "VisibilityShaderInterface.h"
#include "ReconstructShaderInterface.h"
#include "SubdivideShaderInterface.h"
#include "GradientShaderInterface.h"
#include "QuadMesh.h"
#include "Camera.h"

#include <stdio.h>


#define SCENE_FILE "./scenefiles/cornell_box_notess.txt"

#define SHOOTER_LEVEL 0
#define MAX_SUBDIVIDE_LEVEL 2
#define GRADIENT_THRESHOLD 300.0f

#define CAMERA_MOVE_SPEED 3.0f		// dist per sec
#define CAMERA_ROTATE_SPEED 0.2f	// deg per pixel

class Scene {
private:

	SceneShaderInterface ssi;
	VisibilityShaderInterface vsi;
	ReconstructShaderInterface rsi;
	SubdivideShaderInterface susi;
	GradientShaderInterface gsi;

	QuadMesh quadMesh;

	Camera camera;

	int windowWidth, windowHeight;

public:

	Scene();

	int init();

	void onResize(int w, int h);

	void update(GLFWwindow *window, double delta);
	void render();

	void close();
};