NOTE: Radiosity.exe requires OpenGL 4.0 or above.
To run, enter in command prompt one of the following:

Radiosity.exe
Radiosity.exe scenefiles/scene.txt

If a scene file is not specified, it will use the default:
./scenefiles/cornell_box.txt


Controls: (Focus must be on main window, not command prompt)
	
	'F'		- toggle wireframe mode on/off
				(doesn't do anything while radiosity is being calculated)
	
	'G'		- toggle texture filtering between trilinear and nearest
	
	'Enter'	- start radiosity calculations
	
	'X'		- stop radiosity calculations
				(must hold down until end of current shoot iteration)
				
	'W', 'A', 'S', 'D'		- move camera around
							(doesn't work well while radiosity is being calculated)
							
	Left Mouse				- hold and move mouse to look around
							(doesn't work well while radiosity is being calculated)
							
							
