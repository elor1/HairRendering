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
	//ImGui::ShowDemoWindow();
	ImGui::Begin("Settings");
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
	Hair* hair = mApp->GetHair();

	//Update stats
	//ImGui::Text("Stats:");

	double fps = (1.0 / mApp->GetDeltaTime());
	ImGui::Text((std::to_string(fps) + " FPS").c_str());

	int numGuideHairs = hair->GetGuideHairs().size();
	int numGroupHairs = hair->GetNumGroupHairs();
	std::string stats = std::to_string(numGuideHairs) + " simulated guide hairs \n" + std::to_string(numGuideHairs * numGroupHairs) + " total hairs \n";
	ImGui::Text(stats.c_str());

	if (ImGui::CollapsingHeader("Simulation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputFloat3("Wind direction", &hair->mSimulation->windDirection.x);
		ImGui::SliderFloat("Wind strength", &hair->mSimulation->windStrength, 0.0f, 50.0f);
		ImGui::Checkbox("Shake", &hair->mSimulation->shake);
		ImGui::Checkbox("Nod", &hair->mSimulation->nod);
		ImGui::SliderFloat("Stiffness", &hair->mSimulation->stiffness, 0.0f, 1.0f);
		ImGui::SliderFloat("Dampening", &hair->mSimulation->dampening, 0.0f, 1.0f);
		/*ImGui::Checkbox("Friction", &hair->mSimulation->useFriction);
		ImGui::SliderFloat("Friction amount", &hair->mSimulation->friction, 0.0f, 0.5f);*/

		std::string pauseLabel = "Pause";
		if (mApp->IsPaused())
		{
			pauseLabel = "Unpause";
		}

		if (ImGui::Button(pauseLabel.c_str()))
		{
			mApp->TogglePause();
		}

		/*if (ImGui::Button("Reset position"))
		{
			hair->mSimulation->ResetPosition();
		}*/

		if (ImGui::Button("Reset simulation"))
		{
			mApp->ResetSimulation();
		}
	}

	if (ImGui::CollapsingHeader("Hair", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* models[] = { "head.obj", "bear.obj" };
		int currentModel = 0;
		if (ImGui::Combo("Model", &currentModel, models, IM_ARRAYSIZE(models)))
		{
			mApp->SetModel(models[currentModel]);
			mApp->ResetSimulation();
		}

		const char* hairMaps[] = { "hair.png", "hair2.png", "beard.png", "hairBeard.png", "sideburns.png", "black.png"};
		int currentHairMap = 0;
		if (ImGui::Combo("Hair map", &currentHairMap, hairMaps, IM_ARRAYSIZE(hairMaps)))
		{
			mApp->SetHairMap(hairMaps[currentHairMap]);
			mApp->ResetSimulation();
		}
		ImGui::Checkbox("Shadows", &mApp->useShadows);
		ImGui::Checkbox("Transparency", &mApp->useTransparency);
		if (!mApp->useTransparency)
		{
			ImGui::Checkbox("Supersampling", &mApp->useSuperSampling);
		}
		if (ImGui::SliderFloat("Density", &mApp->hairDensity, 0.0f, 1000.0f))
		{
			mApp->ResetSimulation();
		}
		ImGui::SliderInt("Strands per guide hair", &hair->mNumGroupHairs, 0, 64);
		ImGui::SliderInt("Vertices per strand", &hair->mNumSplineVertices, 0, 50);
		if (ImGui::SliderFloat("Max length", &mApp->maxLength, 0.01, 2))
		{
			mApp->ResetSimulation();
		}
		ImGui::SliderFloat("Hair radius", &hair->mHairRadius, 0.0f, 0.1f);
		ImGui::SliderFloat("Group spread", &hair->mGroupSpread, 0.0f, 1.0f);
		ImGui::ColorEdit3("Hair colour", &hair->mColour.x);
		ImGui::SliderFloat("Noise amplitude", &hair->mNoiseAmplitude, 0.0f, 1.0f);
		ImGui::SliderFloat("Noise frequency", &hair->mNoiseFrequency, 0.0f, 1.0f);
		ImGui::SliderFloat("Shadow intensity", &hair->mShadowIntensity, 0.0f, 50.0f);
		ImGui::SliderFloat("Diffuse intensity", &hair->mDiffuseIntensity, 0.0f, 5.0f);
		ImGui::SliderFloat("Specular intensity", &hair->mSpecularIntensity, 0.0f, 5.0f);
		ImGui::SliderFloat("Opacity", &hair->mOpacity, 0.0f, 1.0f);
		ImGui::SliderFloat("Colour variation", &hair->mColourChange, 0.0f, 5.0f);
	}

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiWindow::SetApplication(Application* app)
{
	mApp = app;
}
