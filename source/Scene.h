#pragma once

#include "glincludes.h"
#include "VisibilityShaderInterface.h"
#include "ReconstructShaderInterface.h"
#include "GradientShaderInterface.h"
#include "SubdivideShaderInterface.h"
#include "SceneShaderInterface.h"
#include "QuadMesh.h"
#include "Camera.h"

#include <stdio.h>


#define SCENE_FILE "./scenefiles/cornell_box_notess.txt"

#define SHOOTER_LEVEL 2
#define GRADIENT_THRESHOLD 0.15f		// pixel is dropped if change in slope is greater than this
#define MAX_SUBDIVIDE_LEVEL 3

#define CAMERA_MOVE_SPEED 3.0f		// dist per sec
#define CAMERA_ROTATE_SPEED 0.2f	// deg per pixel

class Scene {
private:

	VisibilityShaderInterface vsi;
	ReconstructShaderInterface rsi;
	GradientShaderInterface gsi;
	SubdivideShaderInterface susi;
	SceneShaderInterface ssi;

	QuadMesh quadMesh;

	Camera camera;

	int windowWidth, windowHeight;


	bool wireframe;

	bool started;
	bool converged;

public:

	Scene();

	void init();

	void onResize(int w, int h);

	void update(GLFWwindow *window, double delta);
	void render();

	void close();

private:

	void enableWireframeMode(bool en);
};