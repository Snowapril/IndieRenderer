/**
* @file		EngineApp.hpp
* @author	Shinjihong
* @date		15 July 2018
* @version	1.0.0
* @brief	Engine class which is main of this program.
* @details	All rendering techniques will be done here.
* @see		GLApp.hpp
*/

#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "GLApp.hpp"
#include "GLMesh.hpp"
#include "GLModel.hpp"
#include <memory>
#include "GLShader.hpp"
#include "EngineCamera.hpp"
#include "EngineGUI.hpp"

class EngineApp : public GLApp, EngineGUI
{
private:
	/// Sphere, loaded with my custom 3D Loader.
	GLModel sphere;
	GLMesh testMesh; 
	/// shader for deferred rendering.
	std::shared_ptr<GLShader> deferredShader; 
	/// shader for G Buffer.
	std::shared_ptr<GLShader> gBufferShader;  
	/// shader for simple test
	std::shared_ptr<GLShader> simpleShader;
	/// shader for Physically Based Rendering
	std::shared_ptr<GLShader> pbrShader;

	/// uniform buffer which have view, projection matrix.
	unsigned int vpUBO;  
	/// IndieEngine Camera, rotation, zoom will be provioded. moving is not supoorted .
	EngineCamera camera; 

	/// Light sources' position vector.
	std::vector<glm::vec3> lightPositions; 
	/// Light sources' color.
	std::vector<glm::vec3> lightColors;    
	

	GLuint albedoMap;
	GLuint normalMap;
	GLuint metallicMap;
	GLuint roughnessMap;
	GLuint aoMap;

	/// Framebuffer for G Buffer.
	GLuint gBuffer;     
	/// Position buffer
	GLuint gPosition;  
	/// Normal buffer
	GLuint gNormal;    
	/// Albedo & Specular buffer which is floating point buffer for HDR. components RGBA is partitioned to RGB and A. RGB is for Color, A is for specular.
	GLuint gColorSpec;  
	/// simple Quad vertex array object for rendering framebuffer texture .
	GLuint simpleQuad;  
private:
	/// bind geometry buffers, that is, bind geometry data in vertex array objects.
	bool buildGeometryBuffers(void); 
	/// bind uniform buffers
	bool buildUniformBuffers(void);  
	/// compile & link all shader files.
	bool buildShaderFiles(void);     
	/// bind framebuffer for the engine. G-Buffer will be binded in this case.
	bool buildFramebuffer(void);	 
	/// push loght sources' data into the light sources' vector/
	bool setupLightSources(void);    
	/// loading all textures needed.
	bool loadTextures(void);         

protected:

public:
	EngineApp();
	virtual ~EngineApp();

	/// all initialization for engine, for example, buildGeometryBuffers, buildUniformBuffers, will be done here.
	bool init(void) override;  

	/// update scene with delta time. delta time will be provided from EngineTimer.
	void updateScene(float dt) override; 
	/// draw scene.
	void drawScene(void) const override; 

	void keyCallback(int key, int scancode, int action, int mode) override;
	void mousePosCallback(double xpos, double ypos) override;
	void mouseBtnCallback(int btn, int action, int mods) override;
	void scrollCallback(double xoffset, double yoffset) override;
	void resizingCallback(int newWidth, int newHeight) override;
};

#endif