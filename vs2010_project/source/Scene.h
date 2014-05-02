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


#define DEFAULT_SCENE_FILE "./scenefiles/cornell_box.txt"

#define CONVERGE_RES_THRESHOLD 0.7f		// shoot-iterations stop when max avg residual irradiance magnitude is less than this value
#define NO_DIVIDE_ITER_THRESHOLD 8		// if this many shoot iterations occur with no subdivision, then subdivision checks are disabled

#define SHOOTERS_PER_UNIT_DIST 8.0f		// a target shooter cell density for shooter quads
#define MAX_SUBDIVIDE_LEVEL 3			// maximum depth of any texture quadtree resulting from subdivision

#define GRADIENT_THRESHOLD 0.35f		// pixel is dropped in gradient shader if change in slope is greater than this


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

	void init(char *sceneFilepath);

	void onResize(int w, int h);

	void update(GLFWwindow *window, double delta);
	void render();

	void close();

private:

	void enableWireframeMode(bool en);
};