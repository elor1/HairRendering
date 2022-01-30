#pragma once
#include "ShaderProgram.h"

class FeedbackShaderProgram : public ShaderProgram
{
public:
	FeedbackShaderProgram(const char** varyings, int numVaryings, const char* vertex, const char* fragment = "", const char* geometry = "", const char* tessControl = "", const char* tessEval = "");

protected:
	GLuint Load(const char** varyings, int numVaryings);
	GLuint CreateProgram(std::vector<GLuint>& shaders, const GLchar** varyings, int numVaryings);
};