#include "GuiWindow.h"
#include "Application.h"
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

	//Update FPS
	double fps = (1.0 / mApp->GetDeltaTime());
	ImGui::Text((std::to_string(fps) + " FPS").c_str());

	if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}

	if (ImGui::CollapsingHeader("Geometry"))
	{

	}

	if (ImGui::CollapsingHeader("Rendering"))
	{

	}

	//Update stats
	Hair* hair = mApp->GetHair();

	int numGuideHairs = hair->GetGuideHairs().size();
	int numGroupHairs = hair->GetNumGroupHairs();
	std::string stats = std::to_string(numGuideHairs) + " guide hairs \n" + std::to_string(numGroupHairs * numGroupHairs) + " total hairs \n";
	ImGui::Text(stats.c_str());

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiWindow::SetApplication(Application* app)
{
	mApp = app;
}
