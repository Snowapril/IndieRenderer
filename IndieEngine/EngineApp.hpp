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
	GLModel sphere;
	GLMesh testMesh;
	std::shared_ptr<GLShader> deferredShader;
	std::shared_ptr<GLShader> gBufferShader;

	unsigned int vpUBO;
	EngineCamera camera;

	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	float exposure;

	GLuint containerDiff;
	GLuint containerSpec;

	GLuint gBuffer;
	GLuint gPosition;
	GLuint gNormal;
	GLuint gColorSpec;
	GLuint simpleQuad;
private:
	bool buildGeometryBuffers(void);
	bool buildUniformBuffers(void);
	bool buildShaderFiles(void);
	bool buildFramebuffer(void);
	bool setupLightSources(void);
	bool loadTextures(void);

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