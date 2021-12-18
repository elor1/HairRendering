#include "Model.h"

Model::Model(std::string filename)
{
	LoadModel(filename);
}

void Model::Draw()
{
	for (auto& mesh : mMeshes)
	{
		mesh.Draw();
	}
}

std::vector<Mesh*> Model::GetMeshes()
{
	std::vector<Mesh*> meshes;
	for (auto& mesh : mMeshes)
	{
		meshes.push_back(&mesh);
	}

	return meshes;
}

Mesh* Model::GetFirstMesh()
{
	return &mMeshes[0];
}

void Model::LoadModel(std::string filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR " << importer.GetErrorString() << std::endl;
		return;
	}
	mDirectory = filename.substr(0, filename.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		mMeshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	/*for (int i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		temp_vertices.push_back(position);

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			temp_uvs.push_back(texCoord);
		}

		if (mesh->HasNormals())
		{
			glm::vec3 normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			temp_normals.push_back(normal);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			vertexIndices.push_back(face.mIndices[0]);
			uvIndices.push_back(face.mIndices[1]);
			normalIndices.push_back(face.mIndices[2]);
		}
	}

	for (int i = 0; i < vertexIndices.size(); i++)
	{
		int vertexIndex = vertexIndices[i];
		int texCoordIndex = uvIndices[i];
		int normalIndex = normalIndices[i];

		vertices.push_back(Vertex(temp_vertices[vertexIndex - 1], temp_uvs[texCoordIndex - 1], temp_normals[normalIndex - 1]));
	}*/

	return Mesh(vertices, indices);
}
