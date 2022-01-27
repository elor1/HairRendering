#pragma once
#include "ShaderProgram.h"

class MeshShaderProgram : public ShaderProgram
{
public:
	MeshShaderProgram(const char* vertex = "src/shaders/mesh.vert", const char* fragment = "src/shaders/mesh.frag");

	virtual void SetGlobalUniforms() override;
	virtual void SetObjectUniforms() override;
};