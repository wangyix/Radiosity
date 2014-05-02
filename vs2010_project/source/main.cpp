#include <stdio.h>
#include <stdlib.h>

#include "glincludes.h"

#include "Scene.h"
#include "Utils.h"


#define WINDOW_TITLE "Radiosity"


Scene scene;

int frameCount = 0;
int fps = 0;


void init(char *sceneFilepath);
void reshape( GLFWwindow* window, int w, int h );
void update(double delta);
void display( void );
void close();

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );


void init(char *sceneFilepath)
{
	// initialize GLEW
	GLenum GlewInitResult = glewInit();
	if (GlewInitResult != GLEW_OK) {
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "INFO: OpenGL Version: %s\n\n", glGetString(GL_VERSION));

	scene.init(sceneFilepath);
}


void reshape( GLFWwindow* window, int w, int h )
{
	scene.onResize(w, h);
}

void update(GLFWwindow* window, double delta) {

	// calculate fps
	static double elapsed = 0.0;
	elapsed += delta;
	if (elapsed >= 0.25) {
		fps = frameCount*4;
		elapsed = 0.0;
		frameCount = 0;
	}
	scene.update(window, delta);
}


void display(void)
{
	scene.render();

	frameCount++;
}

void close() {
	scene.close();
}




void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}





int main(int argc, char *argv[])
{
	char *sceneFilepath;
	if (argc<=1) {
		printf("Using default scene file %s!\n", DEFAULT_SCENE_FILE);
		sceneFilepath = DEFAULT_SCENE_FILE;
	} else {
		sceneFilepath = argv[1];
	}

	GLFWwindow* window;
	int width, height;

	// init GLFW
	if( !glfwInit() )
	  exit( EXIT_FAILURE );

	glfwWindowHint(GLFW_DEPTH_BITS, 16);

	window = glfwCreateWindow( 800, 600, WINDOW_TITLE, NULL, NULL );
	if (!window)
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval( 1 );

	glfwGetFramebufferSize(window, &width, &height);
	reshape(window, width, height);

	glfwSetTime( 0.0 );

	init(sceneFilepath);

	
	// main loop
	
	double t;
	double t_old = 0.f;
	double dt;

	int titleBufferLength = strlen(WINDOW_TITLE)+20;
	char *windowTitle = new char[titleBufferLength];

	for (;;)
	{
		// calculate delta
		t = glfwGetTime();
		dt = t - t_old;
		t_old = t;

		// render
		update(window, dt);
		display();

		// update fps counter in window title
		sprintf_s(windowTitle, titleBufferLength,
				"%s    FPS: %d", WINDOW_TITLE, fps);
		glfwSetWindowTitle(window, windowTitle);
		
		glfwSwapBuffers(window);
		glfwPollEvents();

		
		// check if we should stop running
		if (glfwWindowShouldClose(window))
			break;
	}

	delete[] windowTitle;

	close();

	glfwTerminate();
	exit( EXIT_SUCCESS );
}

