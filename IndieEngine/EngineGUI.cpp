#include "EngineGUI.hpp"
#include <glad/glad.h>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <imgui\imgui_internal.h>

using namespace ImGui;

enum class Material : int
{
	GRASS = 0,
		GOLD = 1,
		SILVER = 2,
		RUSTED_IRON = 3,
		PLASTIC = 4,
		WALL = 5,
		WOOD = 6
};

enum class Model : int
{
	SHADER_BALL = 0,
	SPHERE = 1
};

enum class Environment : int
{
	HOUSE = 0,
	FACTORY = 1,
	LAKE = 2,
	CITY = 3
};

EngineGUI::EngineGUI()
	: isGUIOpen(true), exposure(0.5f), gamma(2.2f), rotationVelocity(1.f), scaleRatio(1.f), toneMappingMode(1),
		useReinhard(true), materialIdx(0), modelIdx(0), environmentIdx(0)
{

}

EngineGUI::~EngineGUI()
{
	ImGui_ImplGlfwGL3_Shutdown();
}


bool EngineGUI::initGUI(GLFWwindow* window)
{
	if (!ImGui_ImplGlfwGL3_Init(window, true))
		return false;

	return true;
}

void EngineGUI::updateGUI(float dt, float height)
{
	ImGui_ImplGlfwGL3_NewFrame();

	ImGui::Begin("Indie Engine", &isGUIOpen, ImVec2(0, 0), 0.5f, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoSavedSettings);
	ImGui::SetWindowSize(ImVec2(350, height));

	//GUI setup code here.
	if (ImGui::TreeNode("Post Processing"))
	{
		if (ImGui::TreeNode("HDR"))
		{
			ImGui::RadioButton("Reinhard", &toneMappingMode, 1);
			ImGui::RadioButton("Use exposure", &toneMappingMode, 2);

			useReinhard = (toneMappingMode == 1);

			if (!useReinhard)
				ImGui::SliderFloat("Exposure", &exposure, 0.0f, 1.0f);

			ImGui::TreePop();
		}
		ImGui::SliderFloat("Gamma", &gamma, 0.0f, 10.f);

		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Camera"))
	{
		if (ImGui::TreeNode("Transformation"))
		{
			ImGui::SliderFloat("Rotation Speed", &rotationVelocity, 0.0f, 5.f);
			ImGui::SliderFloat("Scale", &scaleRatio, 0.0f, 3.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Model"))
		{
			ImGui::RadioButton("Shader Ball", &modelIdx, static_cast<int>(Material::GRASS));
			ImGui::RadioButton("Sphere", &modelIdx, static_cast<int>(Material::GOLD));

			ImGui::TreePop();

		}
		if (ImGui::TreeNode("Material"))
		{
			ImGui::RadioButton("Grass", &materialIdx, static_cast<int>(Material::GRASS));
			ImGui::RadioButton("Gold", &materialIdx, static_cast<int>(Material::GOLD));
			ImGui::RadioButton("Silver", &materialIdx, static_cast<int>(Material::SILVER));
			ImGui::RadioButton("Rusted Iron", &materialIdx, static_cast<int>(Material::RUSTED_IRON));
			ImGui::RadioButton("Plastic", &materialIdx, static_cast<int>(Material::PLASTIC));
			ImGui::RadioButton("Wall", &materialIdx, static_cast<int>(Material::WALL));
			ImGui::RadioButton("Wood", &materialIdx, static_cast<int>(Material::WOOD));

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Environment"))
		{
			ImGui::RadioButton("House", &environmentIdx, static_cast<int>(Environment::HOUSE));
			ImGui::RadioButton("Factory", &environmentIdx, static_cast<int>(Environment::FACTORY));
			ImGui::RadioButton("Lake", &environmentIdx, static_cast<int>(Environment::LAKE));
			ImGui::RadioButton("City", &environmentIdx, static_cast<int>(Environment::CITY));

			ImGui::TreePop();
		}
		
		ImGui::TreePop();
	}

	if (ImGui::CollapsingHeader("Key Support", 0, true, true))
	{
		ImGui::Text("Enter : Window/Full screen");
		ImGui::Text("RMB   : Zoom in/out");
	}

	if (ImGui::CollapsingHeader("Profiling", 0, true, true))
	{
		ImGui::Text("Delta time %.2f ms", dt);
	}

	if (ImGui::CollapsingHeader("Application Info", 0, true, true))
	{
		char* glInfos = (char*)glGetString(GL_VERSION);
		char* hardwareInfos = (char*)glGetString(GL_RENDERER);

		ImGui::Text("OpenGL Version :");
		ImGui::Text(glInfos);
		ImGui::Text("Hardware Informations :");
		ImGui::Text(hardwareInfos);
		ImGui::Text("\nFramerate %.2f FPS / Frametime %.4f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	}

	if (ImGui::CollapsingHeader("About", 0, true, true))
	{
		ImGui::Text("IndieEngine by Snowapril");
		ImGui::Text("Email: sinjihng@naver.com");
	}

	ImGui::End();
}

void EngineGUI::renderGUI(void) const
{
	Render();
}
