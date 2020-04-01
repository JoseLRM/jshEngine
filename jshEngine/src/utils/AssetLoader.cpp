#include "..//assimp/Importer.hpp"
#include "..//assimp/postprocess.h"
#include "..//assimp/scene.h"

#include "..//Debug.h"
#include "..//graphics/Graphics.h"
#include "AssetLoader.h"
#include <sstream>

#include "stb_lib.h"

namespace jshLoader
{

	jsh::Mesh LoadMesh(const aiMesh* aimesh, aiMaterial** const materials, const std::string& name)
	{
		jsh::Mesh mesh;
		mesh.rawData = jshGraphics::CreateRawData(name.c_str());

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

		// texture coords
		float* textureCoords = nullptr;
		if (aimesh->HasTextureCoords(0)) {
			uint32 cantTexCoords = aimesh->mNumVertices;
			textureCoords = new float[cantTexCoords * 2u];
			for (uint32 i = 0; i < cantTexCoords; ++i) {
				textureCoords[i * 2u] = aimesh->mTextureCoords[0][i].x;
				textureCoords[i * 2u + 1u] = aimesh->mTextureCoords[0][i].y;
			}
		}

		const aiMaterial* material = materials[aimesh->mMaterialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			jsh::Texture diffuseMap = jshLoader::LoadTexture(path.C_Str());
		}

		// mesh creation
		if (textureCoords) mesh.rawData->SetTextureCoords(textureCoords);
		mesh.rawData->SetIndices(iData, indexCount);
		mesh.rawData->SetPositionsAndNormals((float*)vertices, (float*)normals, aimesh->mNumVertices);
		mesh.rawData->Create();

		if (textureCoords) delete[] textureCoords;

		return mesh;
	}

	jsh::Model* LoadModel(const char* path, const char* name)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

		if (!scene || !scene->HasMeshes()) {
			jshLogE("Empty model %s", path);
			return false;
		}

		uint32 numOfMeshes = scene->mNumMeshes;
		jsh::Model* model = jshGraphics::CreateModel(name);
		model->meshes.reserve(numOfMeshes);

		for (uint32 i = 0; i < numOfMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];

			if (!mesh->HasPositions() || !mesh->HasNormals()) {
				jshLogE("Invalid mesh, %s[%u]", path, i);
				continue;
			}
			std::stringstream ssName;
			ssName << name;
			ssName << '[';
			ssName << i;
			ssName << ']';
			model->meshes.push_back_nr(LoadMesh(mesh, scene->mMaterials, ssName.str()));
		}

		return model;
	}

	jsh::Texture LoadTexture(const char* path)
	{
		int width, height, bits;
		byte* data = stbi_load(path, &width, &height, &bits, 4);

		uint32 pitch = width * 4u;

		return jshGraphics::CreateTexture(data, pitch, width, height, JSH_FORMAT_R8G8B8A8_UNORM);
	}

}