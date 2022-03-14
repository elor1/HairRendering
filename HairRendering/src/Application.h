#pragma once
#include "GuiWindow.h"
#include <glew.h>
#include <glfw3.h>
#include "shaderPrograms/ShaderProgram.h"
#include "Camera.h"
#include "Hair.h"
#include "Model.h"
#include <string>

class Simulation;
class Texture;
class Framebuffer;

class Application
{
public:
	Application(int width, int height);
	~Application();

	void Run();

	//Gui
	double GetDeltaTime();
	Hair* GetHair();
	void TogglePause();
	void ResetSimulation();
	bool IsPaused();
	void SetHairMap(std::string filename);
	void SetModel(std::string filename);
	void SetPreset(std::string preset);
	bool useShadows;
	bool useSuperSampling;
	bool useTransparency;
	float hairDensity;
	float maxLength;

private:
	void UpdateSettings();
	void Initialise();
	void InitSimulation();
	void Draw();
	void Update();
	void ProcessInput();
	static void FrameBufferCallback(GLFWwindow* window, int width, int height);
	void MouseCallback(GLFWwindow* window, double xPos, double yPos);
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void MouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	void DrawMesh(ShaderProgram* program, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
	void DrawHair(ShaderProgram* program, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
	
	Model* mHead;
	Model* mCollider;

	Hair* mHair;
	Simulation* mSimulation;

	std::string mHairMapName;
	std::string mModelName;
	std::string mColliderName;

	std::vector<ShaderProgram*> mPrograms;
	ShaderProgram* mMeshProgram;
	ShaderProgram* mHairProgram;
	ShaderProgram* mHairOpacityProgram;
	ShaderProgram* mWhiteHairProgram;
	ShaderProgram* mWhiteMeshProgram;
	ShaderProgram* mHairDepthPeelProgram;
	ShaderProgram* mMeshDepthPeelProgram;

	Texture* mNoiseTexture;

	std::vector<Framebuffer*> mFramebuffers;
	Framebuffer* mHairShadowFramebuffer;
	Framebuffer* mMeshShadowFramebuffer;
	Framebuffer* mOpacityMapFramebuffer;
	Framebuffer* mSsFramebuffer;
	Framebuffer* mDepthPeelFramebuffer;
	Framebuffer* mDepthPeelFramebuffer1;

	Camera* mCamera;
	bool mFirstMouse;
	double mLastX;
	double mLastY;

	GLFWwindow* mWindow;
	int mWidth;
	int mHeight;

	glm::vec3 mLightPosition;
	glm::mat4 mDirToLight;

	double mPrevTime;
	double mCurrentTime;
	double mDeltaTime;
	int mFrame;

	bool mIsPaused;
	bool mIsSpaceDown;

	GuiWindow* mGui;
};