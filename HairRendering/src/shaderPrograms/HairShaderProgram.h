#pragma once
#include "ShaderProgram.h"

class HairShaderProgram : public ShaderProgram
{
public:
	HairShaderProgram(const char* vertex = "src/shaders/hair.vert", const char* fragment = "src/shaders/hair.frag", const char* geometry = "src/shaders/hair.geom", const char* tessControl = "src/shaders/hair.tcs", const char* tessEval = "src/shaders/hair.tes");
	virtual void SetGlobalUniforms() override;
};