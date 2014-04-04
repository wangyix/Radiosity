#include "Scene.h"

Scene::Scene()
	: ssi(0), quads(0){
}

int Scene::init() {
	
	if (!quads) {

		// TEST!!!!
		numQuads = 6;
		quads = new Quad[6];
		glm::vec3 cubeCorners[8];
		cubeCorners[0] = glm::vec3(0.0f, 0.0f, 0.0f);
		cubeCorners[1] = glm::vec3(1.0f, 0.0f, 0.0f);
		cubeCorners[2] = glm::vec3(1.0f, 1.0f, 0.0f);
		cubeCorners[3] = glm::vec3(0.0f, 1.0f, 0.0f);
		cubeCorners[4] = glm::vec3(0.0f, 0.0f, 1.0f);
		cubeCorners[5] = glm::vec3(1.0f, 0.0f, 1.0f);
		cubeCorners[6] = glm::vec3(1.0f, 1.0f, 1.0f);
		cubeCorners[7] = glm::vec3(0.0f, 1.0f, 1.0f);
		quads[0].setModel(cubeCorners[0], cubeCorners[3], cubeCorners[1]);
		quads[1].setModel(cubeCorners[4], cubeCorners[5], cubeCorners[7]);
		quads[2].setModel(cubeCorners[0], cubeCorners[1], cubeCorners[4]);
		quads[3].setModel(cubeCorners[1], cubeCorners[2], cubeCorners[5]);
		quads[4].setModel(cubeCorners[2], cubeCorners[3], cubeCorners[6]);
		quads[5].setModel(cubeCorners[3], cubeCorners[0], cubeCorners[7]);
		quads[0].setReflectance(glm::vec3(1.0f,0.0f,0.0f));
		quads[1].setReflectance(glm::vec3(0.0f,1.0f,0.0f));
		quads[2].setReflectance(glm::vec3(0.0f,0.0f,1.0f));
		quads[3].setReflectance(glm::vec3(1.0f,1.0f,0.0f));
		quads[4].setReflectance(glm::vec3(0.0f,1.0f,1.0f));
		quads[5].setReflectance(glm::vec3(1.0f,0.0f,1.0f));
	}


	if (!ssi) {
		ssi = new SceneShaderInterface();
		ssi->init();
	}

	camera.setLens(0.1f, 1000.0f, 45.0f);
	camera.setPosition(glm::vec3(5.0f, 3.0f, 4.0f));
	camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

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
}

void Scene::render() {

	ssi->setVertices(Quad::numVertices, Quad::positions, Quad::texcoords,
			Quad::numIndices, Quad::indices);

	for (int i=0; i<numQuads; ++i) {
		ssi->setModelViewProj(camera.getViewProj() * quads[i].getModel());
		ssi->setTexture(quads[i].getRadiosityTex());
		ssi->draw();
	}
}

void Scene::close() {
	if (ssi) {
		ssi->close();
		delete ssi;
	}

	if (quads) {
		delete[] quads;
	}
}