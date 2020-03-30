#include "..//assimp/Importer.hpp"
#include "..//assimp/postprocess.h"
#include "..//assimp/scene.h"

#include "..//Debug.h"
#include "..//graphics/Graphics.h"

namespace jshLoader
{

	jsh::Mesh* LoadMesh(const aiMesh* aimesh)
	{
		// vertex buffer
		aiVector3D* vertices = aimesh->mVertices;
		aiVector3D* normals = aimesh->mNormals;

		// index buffer
		size_t cantOfIndices = aimesh->mNumFaces * 3;
		uint32 indexCount = 0u;
		uint32* iData = new uint32[cantOfIndices];
		for (uint32 i = 0; i < aimesh->mNumFaces; ++i) {
			aiFace& face = aimesh->mFaces[i];
			if (face.mNumIndices != 3) continue;
			iData[indexCount++] = face.mIndices[0];
			iData[indexCount++] = face.mIndices[1];
			iData[indexCount++] = face.mIndices[2];
		}

		// mesh creation
		jsh::Mesh* mesh = new jsh::Mesh();
		mesh->SetIndices(iData, indexCount);
		mesh->SetPositionsAndNormals((float*)vertices, (float*)normals, aimesh->mNumVertices);
		mesh->Create();

		return mesh;
	}

	jsh::Model* LoadModel(const char* path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

		if (!scene || !scene->HasMeshes()) {
			jshLogE("Empty model %s", path);
			return false;
		}

		uint32 numOfMeshes = scene->mNumMeshes;
		jsh::Model* model = new jsh::Model();
		model->meshes.reserve(numOfMeshes);

		for (uint32 i = 0; i < numOfMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];

			if (!mesh->HasPositions() || !mesh->HasNormals()) {
				jshLogE("Invalid mesh, %s[%u]", path, i);
				continue;
			}

			model->meshes.push_back_nr(LoadMesh(mesh));
		}

		return model;
	}

}