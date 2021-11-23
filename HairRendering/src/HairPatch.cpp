#include "HairPatch.h"
#include "ShaderProgram.h"

HairPatch::~HairPatch()
{
	mPatch.Destroy();
}

void HairPatch::Initialise()
{
    /*ShaderProgram* fullShader = new ShaderProgram("../HairRendering/src/shaders/full.vert", "../HairRendering/src/shaders/full.frag", "", "../HairRendering/src/shaders/full.tcs", "../HairRendering/src/shaders/full.tes");
    mFullShader = fullShader->Load();*/

    GLfloat data[] = { -0.5f, 0.5f, 0.0f,
                        0.5f, 0.5f, 0.0f,
                       -0.5f, -0.5f, 0.0f,
                        0.5f, -0.5f, 0.0f };

    mPatch.Create();
    mPatch.SetVertexData(data, sizeof(data), 4);
    mPatch.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void HairPatch::Draw()
{
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    mPatch.Draw(GL_PATCHES);
}

void HairPatch::TestData(GLfloat* data, int numVertices, float time)
{
    for (int i = 0; i < numVertices; i++)
    {
        data[3 * i] = 0.1f * sin(8 * (time + (float)i / numVertices));
        data[3 * i + 1] = -1.0f + 2.0f * i / numVertices;
        data[3 * i + 2] = 0.0f;
    }
}
