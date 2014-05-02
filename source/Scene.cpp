#include "Scene.h"

Scene::Scene()
	: ssi(0), vsi(0), quads(0){
}

int Scene::init() {
	
	if (!quads) {

		// TEST!!!!
		quads = new Quad[96];
		numQuads = 0;
		
		// cube A
		glm::vec3 cubeCorners[12];
		cubeCorners[0] = glm::vec3(-1.5f, -0.8f, 0.0f);
		cubeCorners[1] = glm::vec3(-0.5f, -0.8f, 0.0f);
		cubeCorners[2] = glm::vec3(-0.5f, 0.2f, 0.0f);
		cubeCorners[3] = glm::vec3(-1.5f, 0.2f, 0.0f);
		cubeCorners[4] = glm::vec3(-1.5f, -0.8f, 1.0f);
		cubeCorners[5] = glm::vec3(-0.5f, -0.8f, 1.0f);
		cubeCorners[6] = glm::vec3(-0.5f, 0.2f, 1.0f);
		cubeCorners[7] = glm::vec3(-1.5f, 0.2f, 1.0f);

		quads[numQuads++].setModel(cubeCorners[0], cubeCorners[3], cubeCorners[1]);
		quads[numQuads++].setModel(cubeCorners[4], cubeCorners[5], cubeCorners[7]);

		quads[numQuads++].setModel(cubeCorners[0], cubeCorners[1], cubeCorners[4]);
		quads[numQuads++].setModel(cubeCorners[1], cubeCorners[2], cubeCorners[5]);
		quads[numQuads++].setModel(cubeCorners[2], cubeCorners[3], cubeCorners[6]);
		quads[numQuads++].setModel(cubeCorners[3], cubeCorners[0], cubeCorners[7]);


		cubeCorners[0] = glm::vec3(0.8f, 0.0f, 0.0f);
		cubeCorners[1] = glm::vec3(1.4f, 0.8f, 0.0f);
		cubeCorners[2] = glm::vec3(0.6f, 1.4f, 0.0f);
		cubeCorners[3] = glm::vec3(0.0f, 0.6f, 0.0f);
		cubeCorners[4] = glm::vec3(0.8f, 0.0f, 1.0f);
		cubeCorners[5] = glm::vec3(1.4f, 0.8f, 1.0f);
		cubeCorners[6] = glm::vec3(0.6f, 1.4f, 1.0f);
		cubeCorners[7] = glm::vec3(0.0f, 0.6f, 1.0f);
		cubeCorners[8] = glm::vec3(0.8f, 0.0f, 2.0f);
		cubeCorners[9] = glm::vec3(1.4f, 0.8f, 2.0f);
		cubeCorners[10] = glm::vec3(0.6f, 1.4f, 2.0f);
		cubeCorners[11] = glm::vec3(0.0f, 0.6f, 2.0f);

		quads[numQuads++].setModel(cubeCorners[0], cubeCorners[3], cubeCorners[1]);
		quads[numQuads++].setModel(cubeCorners[8], cubeCorners[9], cubeCorners[11]);
		/*
		quads[numQuads++].setModel(cubeCorners[0], cubeCorners[1], cubeCorners[4]);
		quads[numQuads++].setModel(cubeCorners[1], cubeCorners[2], cubeCorners[5]);
		quads[numQuads++].setModel(cubeCorners[2], cubeCorners[3], cubeCorners[6]);
		quads[numQuads++].setModel(cubeCorners[3], cubeCorners[0], cubeCorners[7]);
		*/
		quads[numQuads++].setModel(cubeCorners[0], cubeCorners[1], cubeCorners[8]);
		quads[numQuads++].setModel(cubeCorners[1], cubeCorners[2], cubeCorners[9]);
		quads[numQuads++].setModel(cubeCorners[2], cubeCorners[3], cubeCorners[10]);
		quads[numQuads++].setModel(cubeCorners[3], cubeCorners[0], cubeCorners[11]);


		cubeCorners[0] = glm::vec3(-2.0f, -2.0f, 0.0f);
		cubeCorners[1] = glm::vec3(2.0f, -2.0f, 0.0f);
		cubeCorners[2] = glm::vec3(2.0f, 2.0f, 0.0f);
		cubeCorners[3] = glm::vec3(-2.0f, 2.0f, 0.0f);
		cubeCorners[4] = glm::vec3(-2.0f, -2.0f, 3.0f);
		cubeCorners[5] = glm::vec3(2.0f, -2.0f, 3.0f);
		cubeCorners[6] = glm::vec3(2.0f, 2.0f, 3.0f);
		cubeCorners[7] = glm::vec3(-2.0f, 2.0f, 3.0f);

		numQuads += Quad::tessellate(&quads[numQuads], cubeCorners[0], cubeCorners[1], cubeCorners[3], 1, 1, glm::vec3(0.0f,1.0f,0.0f));
		numQuads += Quad::tessellate(&quads[numQuads], cubeCorners[4], cubeCorners[7], cubeCorners[5], 1, 1, glm::vec3(0.0f,1.0f,0.0f));

		numQuads += Quad::tessellate(&quads[numQuads], cubeCorners[0], cubeCorners[4], cubeCorners[1], 1, 1, glm::vec3(0.0f,1.0f,0.0f));
		numQuads += Quad::tessellate(&quads[numQuads], cubeCorners[1], cubeCorners[5], cubeCorners[2], 1, 1, glm::vec3(0.0f,1.0f,0.0f));
		numQuads += Quad::tessellate(&quads[numQuads], cubeCorners[2], cubeCorners[6], cubeCorners[3], 1, 1, glm::vec3(0.0f,1.0f,0.0f));
		numQuads += Quad::tessellate(&quads[numQuads], cubeCorners[3], cubeCorners[7], cubeCorners[0], 1, 1, glm::vec3(0.0f,1.0f,0.0f));




		quadPositions = new float[4*3*numQuads];
		quadIds = new unsigned int[4*numQuads];

		Quad::buildWorldVerticesArray(quads, numQuads, quadPositions, quadIds);

	}

	/*
	if (!ssi) {
		ssi = new SceneShaderInterface();
		ssi->init();
	}*/

	if (!vsi) {
		vsi = new VisibilityShaderInterface();
		vsi->init();

		vsi->setNearFar(0.0f, 1000.0f);
		vsi->setVertices(4*numQuads, quadPositions, quadIds);
	}

	camera.setLens(0.1f, 1000.0f, 45.0f);
	camera.setPosition(glm::vec3(0.0f, -5.0f, 1.0f));
	camera.lookAt(glm::vec3(0.0f, 0.0f, 1.0f));

	return 0;
}


void Scene::onResize(int w, int h) {
	camera.setAspect((float)w / (float)h);
}


void Scene::update(GLFWwindow *window, double delta) {

	float forward = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		forward += 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		forward -= 1.0f;
	}
	float right = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		right -= 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		right += 1.0f;
	}
	float up = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		up += 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		up -= 1.0f;
	}
	glm::vec3 move = glm::vec3(forward, right, up);
	if (glm::length(move) != 0.0f) {
		move = (float)delta * CAMERA_MOVE_SPEED * glm::normalize(move);
		camera.moveForward(move.x);
		camera.moveRight(move.y);
		camera.moveUp(move.z);
	}

	static double x, y;
	glfwGetCursorPos(window, &x, &y);
	
	static double prevX=x, prevY=y;
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		float dx = (float)(x-prevX);
		float dy = (float)(y-prevY);
		camera.rotateRight(CAMERA_ROTATE_SPEED*dx);
		camera.rotateUp(-CAMERA_ROTATE_SPEED*dy);
	}
	prevX = x;
	prevY = y;


	static bool wireframe = true;
	static bool fKeyDown = false;

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		fKeyDown = true;
	} else if(fKeyDown) {
		if (wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = false;
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframe = true;
		}
		fKeyDown = false;
	}
}


void Scene::render() {

	/*
	ssi->setVertices(Quad::numVertices, Quad::positions, Quad::texcoords,
			Quad::numIndices, Quad::indices);

	for (int i=0; i<numQuads; ++i) {
		ssi->setModelViewProj(camera.getViewProj() * quads[i].getModel());
		ssi->setTexture(quads[i].getRadiosityTex());
		ssi->draw();
	}*/

	vsi->setModelView(camera.getView());
	vsi->draw();
}

void Scene::close() {
	if (ssi) {
		ssi->close();
		delete ssi;
	}

	if (vsi) {
		vsi->close();
		delete vsi;
	}

	if (quads) {
		delete[] quads;
	}
}