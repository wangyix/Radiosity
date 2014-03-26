#include "Scene.h"

Scene::Scene() {
	soi = 0;
}

int Scene::init() {
	
	if (!soi) {
		soi = new ShaderOneInterface();
		return soi->init();
	}
	return 0;
}

void Scene::update(double delta) {

}

void Scene::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	glutPostRedisplay();
}

void Scene::close() {
	if (soi) {
		soi->close();
		delete soi;
	}
}