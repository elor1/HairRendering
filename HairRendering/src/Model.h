#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Importer.hpp"
#include "scene.h"
#include "postprocess.h"
#include "Mesh.h"

class Model
{
public:
	Model(std::string filename);

	void Draw();
	std::vector<Mesh*> GetMeshes();
	Mesh* GetFirstMesh();

private:
	std::vector<Mesh> mMeshes;
	std::string mDirectory;

	void LoadModel(std::string filename);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
};