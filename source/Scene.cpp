#include "Scene.h"

Scene::Scene() {
	ssi = 0;
}

int Scene::init() {
	
	if (!ssi) {
		ssi = new SceneShaderInterface();
		return ssi->init();
	}
	return 0;
}

void Scene::update(double delta) {

}

void Scene::render() {
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	glutSwapBuffers();
	glutPostRedisplay();
}

void Scene::close() {
	if (ssi) {
		ssi->close();
		delete ssi;
	}
}