#include "Scene.h"

Scene::Scene()
	: vsi(), rsi(), gsi(), susi(), ssi(), quadMesh(),
	windowWidth(0), windowHeight(0),
	fKeyDown(false) {
}

int Scene::init() {

	quadMesh.load(SCENE_FILE);
	
	ssi.init(quadMesh.getNumVertices(), quadMesh.getPositionsArray(),
			quadMesh.getTexcoordsArray(), Quad::numIndices, Quad::indices);
		
	vsi.init(quadMesh.getNumVertices(),
			quadMesh.getPositionsArray(), quadMesh.getIdsArray());
	vsi.setNearFar(0.0001f, 1000.0f);	// do not set near to 0: shooter may render itself in front of everything
	
	rsi.init(Quad::numVertices, Quad::positionsModel, Quad::texcoords, Quad::numIndices, Quad::indices);
	rsi.setVisTexelSize(1.0f/(float)VisibilityShaderInterface::getVisTextureWidth(),
			1.0f/(float)VisibilityShaderInterface::getVisTextureHeight());

	susi.init();

	gsi.init();
	gsi.setThresholdAndRadTexelSize(GRADIENT_THRESHOLD,
			1.0f/(float)Quad::getTexWidth(), 1.0f/(float)Quad::getTexHeight());

	camera.setLens(0.1f, 1000.0f, 45.0f);
	camera.setPosition(glm::vec3(0.0f, -5.0f, 1.0f));
	camera.lookAt(glm::vec3(0.0f, 0.0f, 1.0f));

	return 0;
}


void Scene::onResize(int w, int h) {
	windowWidth = w;
	windowHeight = h;
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


	// test key
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		fKeyDown = true;
	} else {
		// toggle ssi sampler on key release
		if (fKeyDown)
			ssi.toggleSampler();
		fKeyDown = false;
	}
}


void Scene::render() {

	static int shootIteration = 0;
	static bool converged = false;


	bool meshChanged = false;

	if (!converged) {

		int shooterIndex;

		// set shooter as quad with largest residual power
		float maxResPowerMag = -1.0f;
		for (int i=0; i<quadMesh.getNumQuads(); i++) {
		
			glm::vec3 resPower = quadMesh.getQuad(i)->getResidualPower();
			float resPowerMag = glm::length(resPower);
		
			if (resPowerMag > maxResPowerMag) {
				maxResPowerMag = resPowerMag;
				shooterIndex = i;
			}
		}
	

		// stop shooting if max residual power is low enough
		if (maxResPowerMag < 0.5f)	{
printf("final mesh has %d quads\n", quadMesh.getNumQuads());
			converged = true;
		}


		Quad* shooter = quadMesh.getQuad(shooterIndex);

		printf("\n\nshooter quad is id=%d (level=%d), powerMag = %f\n", shooter->getId(), shooter->getSubdivideLevel(), maxResPowerMag);



		// shoot residual irradiance from each shooter cell of this shooter	

		shooter->selectAsShooter(SHOOTER_LEVEL);
		glm::mat4 shooterCellView;
		glm::vec3 shooterCellPower;

		while (shooter->hasNextShooterCell()) {

			printf("\n	shooter row, col = (%d, %d)\n", shooter->getCurrentShooterRow(), shooter->getCurrentShooterCol());
			
			shooter->getNextShooterCellUniforms(&shooterCellView, &shooterCellPower);

			// render visibility texture from shooter's perspective
			vsi.setModelView(shooterCellView);
			vsi.draw();


			// set up shooter uniforms for reconstruction pass
			rsi.setShooterUniforms(shooterCellPower);


			// update each receiving quad's residual
			// more quads may be appended to quadMesh if subdividing occurs
			
			int receiverIndex = 0;
			while (receiverIndex < quadMesh.getNumQuads()) {

				Quad *receiver = quadMesh.getQuad(receiverIndex);
				
				if (receiver == shooter) {
					receiverIndex++;
					continue;
				}


				glm::vec4 normalShooterView4 = shooterCellView *
						glm::vec4(receiver->getN(), 0.0f);

				glm::vec3 normalShooterView = glm::vec3(normalShooterView4.x,
						normalShooterView4.y, normalShooterView4.z);

				rsi.setReceiverUniforms(shooterCellView * receiver->getModel(),
						receiver->getParentId(), receiver->getReflectance(),
						normalShooterView);

				rsi.setTextureUniforms(vsi.getVisTexture(), receiver->getRadiosityTex(), receiver->getResidualTex());

				rsi.draw(receiver->getNextRadiosityTex(), receiver->getNextResidualTex(),
						Quad::getTexWidth(), Quad::getTexHeight());


				bool subdivide;
				
				// TODO: add code to determine if this quad needs to be subdivided
				// run gradient shader on receiver's next rad tex
				
				gsi.setTexture(receiver->getNextRadiosityTex());
				int pixelsDiscarded = gsi.draw(Quad::getTexWidth(), Quad::getTexHeight());

				printf("		Receiver is patch id=%d (level %d).\t%d pixels discarded\n",
						receiver->getId(), receiver->getSubdivideLevel(), pixelsDiscarded);

				subdivide = pixelsDiscarded > 2*Quad::getTexWidth();
				


				if (!subdivide || receiver->getSubdivideLevel() >= MAX_SUBDIVIDE_LEVEL) {

					receiver->swapTextures();

					receiverIndex++;
				
				} else {
					
					printf("			subdividing quad id=%d (level=%d)\n", receiver->getId(), receiver->getSubdivideLevel());

					
					// subdivide this receiver quad in the mesh
					Quad *subQuads[4];
					quadMesh.subdivideQuad(receiverIndex, &subQuads[0], &subQuads[1], &subQuads[2], &subQuads[3]);

					// NOTE: receiver ptr no longer valid at this point. use subQuads[0]
					// NOTE: shooter ptr  no longer valid at this point.


					//for (int j=0; j<4; j++)
						//printf("			created child quad id=%d (level=%d)\n", subQuads[j]->getId(), subQuads[j]->getSubdivideLevel());


					// Copy rad, res values of original quad into the 4 subdivided quads
					// using shader. Bilinear filtering is used to calculate new in-between values

					susi.setUniforms(subQuads[0]->getRadiosityTex(), subQuads[0]->getResidualTex());

					susi.draw(	subQuads[0]->getNextRadiosityTex(), subQuads[0]->getNextResidualTex(),
								subQuads[1]->getNextRadiosityTex(), subQuads[1]->getNextResidualTex(),
								subQuads[2]->getNextRadiosityTex(), subQuads[2]->getNextResidualTex(),
								subQuads[3]->getNextRadiosityTex(), subQuads[3]->getNextResidualTex(),
								Quad::getTexWidth(), Quad::getTexHeight()	);

					for (int j=0; j<4; j++) {	
						subQuads[j]->swapTextures();
					}


					shooter = quadMesh.getQuad(shooterIndex); // recalculate the shooter pointer

					meshChanged = true;

					// do not increment receiverIndex; the current receiver has been replaced with its bottom-left
					// quadrant and needs to go thru the reconstruction pass again
					
				}

			}	// end receiver loop
		
		}	// end shooter-cells loop
		
		shooter->clearResidualTex();

		printf("done with shoot iteration %d\n", shootIteration);
		shootIteration++;

	}	// end if (!converged)




	// render all quads to screen
	
	if (meshChanged) {
		ssi.setVertices(quadMesh.getNumVertices(), quadMesh.getPositionsArray(),
				quadMesh.getTexcoordsArray(), Quad::numIndices, Quad::indices);
		meshChanged = false;
	}
	ssi.preDraw(windowWidth, windowHeight);
	ssi.setModelViewProj(camera.getViewProj());
	for (int i=0; i<quadMesh.getNumQuads(); i++) {

		ssi.setTexture(quadMesh.getQuad(i)->getRadiosityTex());
		ssi.draw(quadMesh.getBaseVertex(i));
	
	}
}



void Scene::close() {
	
	ssi.close();

	vsi.close();

	rsi.close();

	susi.close();

	gsi.close();

	quadMesh.unload();
}