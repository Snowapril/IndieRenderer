# Indie Engine 

**Indie Graphics Engine** implemented with Modern OpenGL and C++

Link to demo : https://www.youtube.com/watch?v=I5qreAkpIwY

my github blog url : https://snowapril.github.io

## Screenshot
<img src=./IndieEngine.jpg width=70%></img>

## Features
* Camera
	- Zoom in/out
	- TODO : Quaternion based camera rotation.
* Model
	- Transformation
		- Scale
		- Rotation Velocity
	- External models loading (obj file / assimp)
	- TODO : custom model loader
* Shaders
	- PBR Material
	- Uniform Buffer Object
	- TODO : G-Buffer
* Material
	- PBR Material
		- Albedo
		- Normal
		- Roughness
		- Metallic
		- AO
* Textures
	- Environment CubeMap
	- HDR
* ETC
	- Switching between Window/Full screen.

## How to build
Honestly, i am not good at Cmake. i would appreciate it if you could properly use below list to your environment.
1. Open CMake GUI.
2. set both source code directory and binaries directory as project root folder.
3. Configure.
4. Generate with default options.
5. Open Generated Project.
6. Build "ALL_BUILD" project.
7. Set "IndieEngine" project as startup project.
8. Run (Ctrl + F5).

## How to Use
When you run IndieEngine, there is GUI at the left side of your application.
There are many options you can control. 

##### Keys
Enter : toggle Full screen mode.

RMB   : Zoom in/out

ESC   : Quit program.

## Dependencies
* glfw3
* glad
* spdlog
* glm
* stb_image
* assimp

## Credits
* Joey de Vries (LearnOpenGL)
* JoshuaSenouf (https://github.com/JoshuaSenouf/GLEngine) - i am largely inspired by this project