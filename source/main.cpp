#include <stdio.h>
#include <stdlib.h>

#include "glincludes.h"

#include "Scene.h"
#include "Utils.h"


#define WINDOW_TITLE "Radiosity"


Scene *scene = 0;

int frameCount = 0;
int fps = 0;


void init( void );
void reshape( GLFWwindow* window, int w, int h );
void update(double delta);
void display( void );
void close();

void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods );


void init( void )
{
	// initialize GLEW
	GLenum GlewInitResult = glewInit();
	if (GlewInitResult != GLEW_OK) {
		fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));


	scene = new Scene();
	scene->init();


	glClearColor( 0.0, 0.0f, 0.0f, 0.0f );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	Utils::exitOnGLError("ERROR: could not set depth testing options");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	Utils::exitOnGLError("ERROR: could not set culling options");
}


void reshape( GLFWwindow* window, int w, int h )
{
	glViewport( 0, 0, (GLsizei)w, (GLsizei)h );
	if (scene)
		scene->onResize(w, h);
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
	scene->update(window, delta);
}


void display(void)
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	scene->render();

	glFlush();
	frameCount++;
}

void close() {
	scene->close();
	delete scene;
	scene = 0;
}




void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}





int main( void )
{
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

	init();

	
	// main loop
	
	double t;
	double t_old = 0.f;
	double dt;

	char *windowTitle = new char[strlen(WINDOW_TITLE)+20];

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
		sprintf(windowTitle, "%s    FPS: %d", WINDOW_TITLE, fps);
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

