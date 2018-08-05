/**
* @file		EngineGUI.hpp
* @author	Shinjihong
* @date		15 July 2018
* @version	1.0.0
* @brief	GUI class for IndieEngine.
* @details	user can control rendering settings through GUI.
* @see		
*/

#ifndef ENGINE_GUI_HPP
#define ENGINE_GUI_HPP

struct GLFWwindow;

enum class Material : int;

class EngineGUI
{
private:
	int toneMappingMode;
protected:
	bool isGUIOpen;
	bool useReinhard;
	/// will be used for tone mapping.
	float exposure;
	float gamma;
	float rotationVelocity;
	float scaleRatio;
	int materialIdx;
public:
	EngineGUI();
	virtual ~EngineGUI();

	bool initGUI(GLFWwindow* window);
	void updateGUI(float dt, float height);
	void renderGUI(void) const;
};


#endif