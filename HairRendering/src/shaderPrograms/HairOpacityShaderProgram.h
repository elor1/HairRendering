#pragma once
#include "ShaderProgram.h"

class HairOpacityShaderProgram : public ShaderProgram
{
public:
	HairOpacityShaderProgram(const char* vertex = "hair.vert", const char* fragment = "hairOpacity.frag", const char* geometry = "hair.geom", const char* tessControl = "hair.tcs", const char* tessEval = "hair.tes");

	virtual void SetGlobalUniforms() override;
};