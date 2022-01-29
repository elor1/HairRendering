#include "GuiWindow.h"
#include "Application.h"
#include "Simulation.h"
#include <string>

GuiWindow::GuiWindow(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 440");
}

GuiWindow::~GuiWindow()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GuiWindow::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GuiWindow::Update()
{
	ImGui::Begin("Settings");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
	Hair* hair = mApp->GetHair();

	//Update stats
	ImGui::Text("Stats:");

	double fps = (1.0 / mApp->GetDeltaTime());
	ImGui::Text((std::to_string(fps) + " FPS").c_str());

	int numGuideHairs = hair->GetGuideHairs().size();
	int numGroupHairs = hair->GetNumGroupHairs();
	std::string stats = std::to_string(numGuideHairs) + " simulated guide hairs \n" + std::to_string(numGuideHairs * numGroupHairs) + " total hairs \n";
	ImGui::Text(stats.c_str());

	if (ImGui::CollapsingHeader("Hair", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderInt("Strands per guide hair", &hair->mNumGroupHairs, 0, 64);
		ImGui::SliderInt("Spline vertices", &hair->mNumSplineVertices, 0, 50);
		ImGui::SliderFloat("Hair radius", &hair->mHairRadius, 0.0f, 0.1f);
		ImGui::ColorEdit3("Hair colour", &hair->mColour.x);
	}

	if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Shake", &hair->mSimulation->shake);
		ImGui::Checkbox("Nod", &hair->mSimulation->nod);

		std::string pauseLabel = "Pause";
		if (mApp->IsPaused())
		{
			pauseLabel = "Unpause";
		}

		if (ImGui::Button(pauseLabel.c_str()))
		{
			mApp->TogglePause();
		}

		if (ImGui::Button("Reset position"))
		{
			hair->mSimulation->ResetPosition();
		}
		
		if (ImGui::Button("Reset simulation"))
		{
			mApp->ResetSimulation();
		}
	}

	if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Shadows", &mApp->useShadows);
		ImGui::Checkbox("Super sampling", &mApp->useSuperSampling);
	}

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiWindow::SetApplication(Application* app)
{
	mApp = app;
}
