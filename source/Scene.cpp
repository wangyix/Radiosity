#include "Scene.h"

Scene::Scene()
	: ssi(), vsi(), rsi(), quadMesh(), shooter(0),
	testFlag(false), testKeyDown(false) {
}

int Scene::init() {

	quadMesh.load("./scenefiles/cornell_box.txt");
	
	ssi.init(quadMesh.getNumVertices(), quadMesh.getPositionsArray(),
		quadMesh.getTexcoordsArray(), Quad::numIndices, Quad::indices);
		
	vsi.init(quadMesh.getNumVertices(),
		quadMesh.getPositionsArray(), quadMesh.getIdsArray());
	vsi.setNearFar(0.0001f, 1000.0f);	// do not set near to 0: shooter may render itself in front of everything
	
	rsi.init(quadMesh.getNumVertices(), quadMesh.getPositionsArray(),
		quadMesh.getTexcoordsArray(), Quad::numIndices, Quad::indices);

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



	// TEST!!!! toggle testFlag if F is pressed
	bool down = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
	if (down && !testKeyDown) {
		testFlag = !testFlag;
	}
	testKeyDown = down;
	
}


void Scene::render() {

	static int shootIteration = 0;	// for testing!!!!!!!!!!!
	static bool converged = false;
	

	if (!converged) {

		// set shooter as quad with largest residual power
		float maxResIrradMag = -1.0f;
		for (int i=0; i<quadMesh.getNumQuads(); i++) {
		
			Quad *quad = &quadMesh.getQuad(i);

			glm::vec3 resIrrad = quad->getResidualAvgIrradiance();
			float resIrradMag = glm::length(resIrrad);
		
			if (resIrradMag > maxResIrradMag) {
				maxResIrradMag = resIrradMag;
				shooter = quad;
			}
		}
	


		if (maxResIrradMag < 0.4f)	{// SET SOME THRESHOLD to stop shooting if residual irradiance low enough
			converged = true;
		}



		// shoot residual irradiance from each shooter cell of this shooter

		shooter->selectAsShooter(5);//5);	// 1x1 shooter cells

		glm::mat4 shooterCellView;
		glm::vec3 shooterCellPower;
		while (shooter->hasNextShooterCell()) {
		
			shooter->getNextShooterCellUniforms(&shooterCellView, &shooterCellPower);
		
			// render visibility texture from shooter's perspective
			vsi.setModelView(shooterCellView);
			vsi.draw();

			// set up shooter uniforms for reconstruction pass
			rsi.setShooterUniforms(shooterCellView, shooterCellPower,
					vsi.getVisTexture());

			// update each receiving quad's residual
			for (int i=0; i<quadMesh.getNumQuads(); i++) {
			
				Quad *receiver = &quadMesh.getQuad(i);

				glm::vec4 normalShooterView4 = shooterCellView *
						glm::vec4(receiver->getN(), 0.0f);

				glm::vec3 normalShooterView = glm::vec3(normalShooterView4.x,
						normalShooterView4.y, normalShooterView4.z);

				rsi.setReceiverUniforms(receiver->getId(), receiver->getReflectance(),
						normalShooterView,
						receiver->getRadiosityTex(), receiver->getResidualTex());

				rsi.draw(quadMesh.getBaseVertex(i), receiver->getNextRadiosityTex(), receiver->getNextResidualTex(),
						Quad::getTexWidth(), Quad::getTexHeight());

				receiver->swapTextures();


				//quadMesh.getQuad(0).printRadTex();
				//quadMesh.getQuad(0).printResTex();
			}
		}
		shooter->clearResidualTex();

		printf("done with shoot iteration %d\n", shootIteration);
		shootIteration++;

	}
	



	// render all quads to screen

	ssi.setModelViewProj(camera.getViewProj());
	for (int i=0; i<quadMesh.getNumQuads(); i++) {

		ssi.setTexture(quadMesh.getQuad(i).getRadiosityTex());
		ssi.draw(quadMesh.getBaseVertex(i));
	
	}

	/*
	if (testFlag) {
		ssi.setModelViewProj(camera.getViewProj());
		for (int i=0; i<quadMesh.getNumQuads(); i++) {
			ssi.setTexture(quadMesh.getQuad(i).getRadiosityTex());
			ssi.draw(quadMesh.getBaseVertex(i));
		}
	} else {
		vsi.setModelView(camera.getView());
		vsi.draw();
	}*/
}

void Scene::close() {
	
	ssi.close();

	vsi.close();

	rsi.close();

	quadMesh.unload();
}