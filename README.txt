Radiosity implementation based on:
"Radiosity on Graphics Hardware". Greg Coombe, Mark J. Harris, Anselmo Lastra, 2004.
http://www.cs.unc.edu/~coombe/research/radiosity/coombe04radiosity.pdf

This is a GPU-based implementation of radiosity, which can simulate global
illumination of a diffuse scene.  This particular implementation also
adaptively subdivides the scene patches based on how dramatically the lighting
changes within it.

The directory "Radiosity_exe" contains a stand-along executable of the project
along with sample scenes for input.  "images" contains the output images that
the program generates from those sample scenes.  "vs2010_project" contains the
source code and the project files.


Radiosity parameters are set as #define macros at the top of Scene.h.

Libraries used:
GLEW    - multiplatform openGL extensions loader
GLFW    - multiiplatform windowing library
GLM     - openGL math library