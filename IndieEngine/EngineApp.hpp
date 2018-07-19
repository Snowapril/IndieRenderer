#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "GLApp.hpp"
#include "GLMesh.hpp"
#include "GLModel.hpp"
#include <memory>
#include "GLShader.hpp"
#include "EngineCamera.hpp"

class EngineApp : public GLApp
{
private:
	GLModel shaderBall;
	GLMesh testMesh;
	std::shared_ptr<GLShader> testShader;
	std::shared_ptr<GLShader> postProcessing;
	unsigned int vpUBO;
	EngineCamera camera;

	GLuint framebuffer;
	GLuint colorBufferTexture;
	GLuint depthStencilRBO;
	GLuint simpleQuad;
private:
	bool buildGeometryBuffers(void);
	bool buildUniformBuffers(void);
	bool buildShaderFiles(void);
	bool buildFramebuffer(void);

protected:

public:
	EngineApp();
	~EngineApp();

	bool init(void) override;

	void updateScene(float dt) override;
	void drawScene(void) const override;

	void keyCallback(int key, int scancode, int action, int mode) override;
	void mousePosCallback(double xpos, double ypos) override;
	void mouseBtnCallback(int btn, int action, int mods) override;
	void scrollCallback(double xoffset, double yoffset) override;
	void resizingCallback(int newWidth, int newHeight) override;
};

#endif