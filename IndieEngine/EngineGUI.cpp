#include "EngineGUI.hpp"
#include <glad/glad.h>
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>

using namespace ImGui;

EngineGUI::EngineGUI()
	: isGUIOpen(true), exposure(1.f), gamma(2.2f), rotationVelocity(1.f), toneMappingMode(1), useReinhard(true)
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
		ImGui::SliderFloat("Rotation Speed", &rotationVelocity, 0.0f, 5.f);

		ImGui::TreePop();
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