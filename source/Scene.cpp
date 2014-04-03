#include "Scene.h"

Scene::Scene()
	: ssi(0), quadMesh(0){
}

int Scene::init() {
	
	if (!quadMesh) {
		quadMesh = new QuadMesh();
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


void Scene::update(double delta) {

}

void Scene::render() {

	for (int i=0; i<quadMesh->numQuads; ++i) {
		ssi->setModelViewProj(camera.getViewProj() * quadMesh->quads[i].getModel());
		ssi->setTexture(quadMesh->quads[i].getRadiosityTex());
		ssi->draw();
	}
}

void Scene::close() {
	if (ssi) {
		ssi->close();
		delete ssi;
	}

	if (quadMesh) {
		delete quadMesh;
	}
}