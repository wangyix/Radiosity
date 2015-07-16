#include "Scene.h"

Scene::Scene()
	: vsi(), rsi(), gsi(), susi(), ssi(),
	quadMesh(), camera(),
	windowWidth(0), windowHeight(0),
	wireframe(false), started(false), converged(false) {
}

void Scene::init(char *sceneFilepath) {
	
	// load scene mesh
	quadMesh.load(sceneFilepath);
	

	// initialize shader interfaces

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


	// gl init

	glClearColor( 0.0, 0.0f, 0.0f, 0.0f );

	//glEnable(GL_DEPTH_TEST);	// handled by shader interfaces
	glDepthFunc(GL_LESS);
	Utils::exitOnGLError("ERROR: could not set depth testing options");

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	Utils::exitOnGLError("ERROR: could not set culling options");

	// wireframe mode is initially enabled
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    wireframe = true;
    ssi.setAllWhite(true);

	// set camera to initial position
	camera.setLens(0.1f, 1000.0f, 45.0f);
	camera.setPosition(glm::vec3(0.0f, -5.0f, 1.0f));
	camera.lookAt(glm::vec3(0.0f, 0.0f, 1.0f));
}


void Scene::enableWireframeMode(bool en) {
	if (!wireframe && en) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		wireframe = true;
		ssi.setAllWhite(true);
        printf("Wireframe mode enabled\n");
	} else if (wireframe && !en) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wireframe = false;
		ssi.setAllWhite(false);
        printf("Wireframe mode disabled\n");
	}
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


	// F key: toggles wireframe mode
	static bool fKeyDown = false;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		fKeyDown = true;
	} else {
		// on key release
		if (fKeyDown) {
			if (!started || converged) {	// cannot toggle during radiosity calculations
				enableWireframeMode(!wireframe);
			}
		}	
		fKeyDown = false;
	}

	// ENTER key: starts radiosity calculations
	static bool enterKeyDown = false;
	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		enterKeyDown = true;
	} else {
		// on key release
		if (enterKeyDown) {
			if (!started && !converged) {
				printf("\nSTARTING radiosity calculations! -------------------------\n");
				started = true;
				enableWireframeMode(false);
			}
		}
		enterKeyDown = false;
	}

	// G key: toggles ssi sampler
	static bool gKeyDown = false;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		gKeyDown = true;
	} else {
		// on key release
		if (gKeyDown)
			ssi.toggleSampler();
		gKeyDown = false;
	}

	// X key: hold to stop radiosity shoot iterations
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		if (started && !converged) {
			printf("\nSTOPPING radiosity calculations! -------------------------\n");
			started = false;
		}
	}
}




void Scene::render() {

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );



	static int shootIteration = 0;
	static int noDivideIterations = 0;

	bool meshChanged = false;

	if (started && !converged) {

		int shooterIndex;

		// set shooter as quad with largest residual power
		float maxResAvgIrradMag = -1.0f;
		for (int i=0; i<quadMesh.getNumQuads(); i++) {
		
			glm::vec3 resAvgIrrad = quadMesh.getQuad(i)->getResidualAvgIrradiance();
			float resAvgIrradMag = glm::length(resAvgIrrad);
		
			if (resAvgIrradMag > maxResAvgIrradMag) {
				maxResAvgIrradMag = resAvgIrradMag;
				shooterIndex = i;
			}
		}
	

		// stop shooting if max residual power is low enough
		if (maxResAvgIrradMag < CONVERGE_RES_THRESHOLD)	{
			converged = true;
		}


		Quad* shooter = quadMesh.getQuad(shooterIndex);

		printf("\n\nshooter quad is id=%d (level=%d), resIrradMag = %f\n", shooter->getId(), shooter->getSubdivideLevel(), maxResAvgIrradMag);


		// calculate the desired shooter grid dimension N (there will be NxN grid of shooter cells for this quad)
		float shootersGridDimension = SHOOTERS_PER_UNIT_DIST *
				std::max(glm::length(shooter->getU()), glm::length(shooter->getV()));
		
		// calculate shooter level for this shooter (level = log2(N));
		int c = *(const int *)&shootersGridDimension;
		int shooterLevel = (c >> 23) - 127;
		if (c & 0x007FFFFF)
			shooterLevel++;

		printf("caculated shooter level = %d\n", shooterLevel);

		// select this quad as the shooter, retrieve appropriate mipmap of restex according to shooter level
		shooter->selectAsShooter(shooterLevel);
		glm::mat4 shooterCellView;
		glm::vec3 shooterCellPower;

		while (shooter->hasNextShooterCell()) {

			printf("	shooter row, col = (%d, %d)\n", shooter->getCurrentShooterRow(), shooter->getCurrentShooterCol());
			
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


				// determine if this quad needs to be subdivided

				bool subdivide;
				if (noDivideIterations <= NO_DIVIDE_ITER_THRESHOLD) {
					
					// run gradient shader on receiver's next rad tex
					gsi.setTexture(receiver->getNextRadiosityTex());
					int pixelsDiscarded = gsi.draw(Quad::getTexWidth(), Quad::getTexHeight());

					//printf("		Receiver is patch id=%d (level %d).\t%d pixels discarded\n",
							//receiver->getId(), receiver->getSubdivideLevel(), pixelsDiscarded);

					subdivide = pixelsDiscarded > 0;//2*Quad::getTexWidth();
				
				} else {
					subdivide = false;
				}
				


				// subdivide this quad if necessary

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

		if (meshChanged)
			noDivideIterations = 0;
		else
			noDivideIterations++;

	}	// end if (!converged)




	// render all quads to screen
	
	if (meshChanged) {
		ssi.setVertices(quadMesh.getNumVertices(), quadMesh.getPositionsArray(),
				quadMesh.getTexcoordsArray(), Quad::numIndices, Quad::indices);
		
	}
	ssi.preDraw(windowWidth, windowHeight);
	ssi.setModelViewProj(camera.getViewProj());
	for (int i=0; i<quadMesh.getNumQuads(); i++) {

		ssi.setTexture(quadMesh.getQuad(i)->getRadiosityTex());
		ssi.draw(quadMesh.getBaseVertex(i));
	
	}




	glFlush();
}





void Scene::close() {
	
	ssi.close();
	vsi.close();
	rsi.close();
	susi.close();
	gsi.close();

	quadMesh.unload();
}