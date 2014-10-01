Radiosity implementation based on:
"Radiosity on Graphics Hardware". Greg Coombe, Mark J. Harris, Anselmo Lastra, 2004.
http://www.cs.unc.edu/~coombe/research/radiosity/coombe04radiosity.pdf

This is a GPU-based implementation of radiosity, which can simulate global
illumination of a diffuse scene.  This particular implementation also
adaptively subdivides the scene patches based on how dramatically the lighting
changes within it.

Radiosity parameters are set as #define macros at the top of Scene.h.

Libraries used:
GLEW    - multiplatform openGL extensions loader
GLFW    - multiiplatform windowing library
GLM     - openGL math library