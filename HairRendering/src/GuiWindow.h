#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Application;

class GuiWindow
{
public:
	GuiWindow(GLFWwindow* window);
	~GuiWindow();

	void NewFrame();
	void Update();
	void SetApplication(Application* app);

private:
	Application* mApp;
};