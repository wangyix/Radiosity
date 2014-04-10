#include "Scene.h"

Scene::Scene()
	: ssi(), vsi(), quadMesh(){
}

int Scene::init() {

	quadMesh.load("./scenefiles/cornell_box.txt");

	ssi.init();
	ssi.setVertices(quadMesh.getNumVertices(), quadMesh.getPositionsArray(),
		quadMesh.getTexcoordsArray(), Quad::numIndices, Quad::indices);

	/*
	vsi.init();
	vsi.setNearFar(0.0f, 1000.0f);
	vsi.setVertices(quadMesh.getNumVertices(),
		quadMesh.getPositionsArray(), quadMesh.getIdsArray());
	*/

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
}


void Scene::render() {

	ssi.setModelViewProj(camera.getViewProj());

	for (int i=0; i<quadMesh.getNumQuads(); i++) {
		ssi.setTexture(quadMesh.getQuad(i).getRadiosityTex());
		ssi.draw(quadMesh.getBaseVertex(i));
	}

	//vsi.setModelView(camera.getView());
	//vsi.draw();
}

void Scene::close() {
	
	ssi.close();

	//vsi.close();

	quadMesh.unload();
}