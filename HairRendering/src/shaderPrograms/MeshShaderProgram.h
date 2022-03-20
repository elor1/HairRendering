#pragma once
#include "ShaderProgram.h"

class MeshShaderProgram : public ShaderProgram
{
public:
	MeshShaderProgram(const char* vertex = "mesh.vert", const char* fragment = "mesh.frag");

	virtual void SetGlobalUniforms() override;
	virtual void SetObjectUniforms() override;
};