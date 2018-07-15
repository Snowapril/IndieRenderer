#pragma once
#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "GLApp.hpp"

class EngineApp : public GLApp
{
private:

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