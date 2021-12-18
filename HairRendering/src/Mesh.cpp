//#include "Mesh.h"
//#include <iostream>
//#include <glew.h>
//
//Mesh::Mesh(std::string filename)
//{
//	Load(filename);
//	Initialise();
//}
//
//void Mesh::Draw()
//{
//	mShape.Draw(GL_TRIANGLES);
//}
//
//void Mesh::Load(std::string filename)
//{
//	Assimp::Importer importer;
//	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
//
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//	{
//		std::cout << "ERROR: " << importer.GetErrorString() << std::endl;
//		return;
//	}
//	mDirectory = filename.substr(0, filename.find_last_of('/'));
//
//	Process(scene->mMeshes[scene->mRootNode->mMeshes[0]], scene);
//}
//
//void Mesh::Process(aiMesh* mesh, const aiScene* scene)
//{
//	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
//	{
//		Vertex vertex;
//		glm::vec3 vector;
//		vector.x = mesh->mVertices[i].x;
//		vector.y = mesh->mVertices[i].y;
//		vector.z = mesh->mVertices[i].z;
//		vertex.position = vector;
//
//		if (mesh->HasNormals())
//		{
//			vector.x = mesh->mNormals[i].x;
//			vector.y = mesh->mNormals[i].y;
//			vector.z = mesh->mNormals[i].z;
//			vertex.normal = vector;
//		}
//
//		if (mesh->mTextureCoords[0])
//		{
//			glm::vec2 vec;
//			vec.x = mesh->mTextureCoords[0][i].x;
//			vec.y = mesh->mTextureCoords[0][i].y;
//			vertex.texCoords = vec;
//		}
//		else
//		{
//			vertex.texCoords = glm::vec2(0.0f);
//		}
//
//		mVertices.push_back(vertex);
//	}
//
//	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
//	{
//		aiFace face = mesh->mFaces[i];
//		for (unsigned int j = 0; j < face.mNumIndices; j++)
//		{
//			mIndices.push_back(face.mIndices[j]);
//		}
//	}
//}
//
//void Mesh::Initialise()
//{
//	for (unsigned int i = 0; i < mVertices.size() / 3; i++)
//	{
//		unsigned int index = i * 3;
//		Triangle triangle(mVertices[index], mVertices[index + 1], mVertices[index + 2]);
//		triangles.push_back(triangle);
//	}
//
//	std::vector<GLfloat> VBOdata;
//	for (auto& vertex : mVertices)
//	{
//		VBOdata.push_back(vertex.position.x);
//		VBOdata.push_back(vertex.position.y);
//		VBOdata.push_back(vertex.position.z);
//		VBOdata.push_back(vertex.texCoords.x);
//		VBOdata.push_back(vertex.texCoords.y);
//		VBOdata.push_back(vertex.normal.x);
//		VBOdata.push_back(vertex.normal.y);
//		VBOdata.push_back(vertex.normal.z);
//	}
//	mShape.Create();
//	mShape.SetVertexData(&VBOdata[0], sizeof(GLfloat) * VBOdata.size(), mVertices.size());
//	mShape.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
//	mShape.SetAttribute(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 3 * sizeof(GLfloat));
//	mShape.SetAttribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 5 * sizeof(GLfloat));
//}

#include "Mesh.h"
#include <glew.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
	mVertices = vertices;
	mIndices = indices;

	SetupMesh();
}

void Mesh::SetupMesh()
{
	for (unsigned int i = 0; i < mVertices.size() / 3; i++)
	{
		unsigned int index = i * 3;
		Triangle triangle(mVertices[index], mVertices[index + 1], mVertices[index + 2]);
		triangles.push_back(triangle);
	}

	//std::vector<GLfloat> VBOdata;
	//for (auto& vertex : mVertices)
	//{
	//	VBOdata.push_back(vertex.position.x);
	//	VBOdata.push_back(vertex.position.y);
	//	VBOdata.push_back(vertex.position.z);
	//	VBOdata.push_back(vertex.texCoords.x);
	//	VBOdata.push_back(vertex.texCoords.y);
	//	VBOdata.push_back(vertex.normal.x);
	//	VBOdata.push_back(vertex.normal.y);
	//	VBOdata.push_back(vertex.normal.z);
	//}
	//mShape.Create();
	////mShape.SetVertexData(&VBOdata[0], sizeof(GLfloat) * VBOdata.size(), mVertices.size());
	///*mShape.SetVertexData(mVertices);
	//mShape.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	//mShape.SetAttribute(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Vertex::texCoords));
	//mShape.SetAttribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Vertex::normal));*/
	//mShape.SetupMesh(mVertices, mIndices);
	///*mShape.SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
	//mShape.SetAttribute(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 3 * sizeof(GLfloat));
	//mShape.SetAttribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 5 * sizeof(GLfloat));*/

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Mesh::Draw()
{
	//mShape.Draw(GL_TRIANGLES);
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

std::vector<Vertex> Mesh::GetVertices()
{
	return mVertices;
}