#pragma once
#include "ShaderProgram.h"

class HairShaderProgram : public ShaderProgram
{
public:
	HairShaderProgram(const char* vertex = "hair.vert", const char* fragment = "hair.frag", const char* geometry = "hair.geom", const char* tessControl = "hair.tcs", const char* tessEval = "hair.tes");
	virtual void SetGlobalUniforms() override;
};