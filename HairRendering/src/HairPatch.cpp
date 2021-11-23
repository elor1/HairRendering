#include "HairPatch.h"

HairPatch::~HairPatch()
{
	mPatch.Destroy();
}

void HairPatch::Initialise()
{
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
    mPatch.Draw(GL_PATCHES);
}
