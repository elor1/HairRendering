#pragma once
#include "ShaderProgram.h"

class HairOpacityShaderProgram : public ShaderProgram
{
public:
	HairOpacityShaderProgram(const char* vertex = "src/shaders/hair.vert", const char* fragment = "src/shaders/hairOpacity.frag", const char* geometry = "src/shaders/hair.geom", const char* tessControl = "src/shaders/hair.tcs", const char* tessEval = "src/shaders/hair.tes");

	virtual void SetGlobalUniforms() override;
};