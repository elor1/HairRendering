#pragma once
#include <glew.h>
#include <vector>
#include <map>
#include <string>
#include <glm.hpp>

struct Uniforms
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	int numGroupHairs;
	int numHairVertices;
	GLfloat vertexData[64];
	float groupWidth;
	glm::vec3 colour;
};

class ShaderProgram
{
public:
	ShaderProgram(const char* vertex, const char* fragment, const char* geometry = "", const char* tessControl = "", const char* tessEval = "");

	GLuint GetID();
	void SetUniforms();
	void Bind();
	void Unbind();

	Uniforms uniforms;

private:
	GLuint CreateShader(GLenum type, const char* path);
	GLuint CreateProgram(std::vector<GLuint>& shaders);
	GLuint Load();

	const char* mVertex;
	const char* mFragment;
	const char* mGeometry;
	const char* mTessControl;
	const char* mTessEval;
	GLuint mID;
	std::map<std::string, int> mUniformLocations;
};