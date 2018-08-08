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