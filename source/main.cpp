#include "glincludes.h"
#include "ShaderLoader.h"
#include "Scene.h"

#include <stdio.h>
#include <Windows.h>


#define WINDOW_TITLE_PREFIX "Radiosity"

int CurrentWidth = 800,
	CurrentHeight = 600,
	WindowHandle = 0;

void Initialize(int, char*[]);
void Close();
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void InitShaders();

void errorPressAnyKeyToExit() {
	fprintf(stderr, "Press enter to exit... ");
	getchar();
	exit(EXIT_FAILURE);
}


Scene *scene = 0;

double PCFreq = 0.0;	// number of counts per ms of QueryPerformanceCounter 



int main(int argc, char* argv[]) {
	Initialize(argc, argv);
	glutMainLoop();
	Close();
	exit(EXIT_SUCCESS);
}


void Initialize(int argc, char* argv[]) {

	InitWindow(argc, argv);
	
	printf("INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	scene = new Scene();
	if (scene->init()) {
		errorPressAnyKeyToExit();
	}
	

	LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li)) {
		fprintf(stderr, "QueryPerformanceFrequency failed!\n");
		errorPressAnyKeyToExit();
	}
    PCFreq = double(li.QuadPart)/1000.0;


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Close() {
	if (scene)
		scene->close();
	delete scene;
}



void InitWindow(int argc, char* argv[]) {
	glutInit(&argc, argv);
	
	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
	);
	
	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);
	if(WindowHandle < 1) {
		fprintf(stderr, "ERROR: Could not create a new rendering window.\n");
		errorPressAnyKeyToExit();
	}

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "ERROR: Could not initialize glew.\n");
		errorPressAnyKeyToExit();
	}

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
}



void ResizeFunction(int Width, int Height) {
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
}


void RenderFunction(void) {

	
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	__int64 currTime = li.QuadPart;
	static __int64 prevUpdateTime = currTime;

	scene->update(double(currTime-prevUpdateTime)/PCFreq);
	scene->render();

	prevUpdateTime = currTime;
}