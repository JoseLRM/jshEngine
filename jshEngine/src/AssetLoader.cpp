#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "Debug.h"
#include "Graphics.h"
#include "AssetLoader.h"
#include <sstream>

#include "stb/stb_lib.h"

namespace jshLoader
{

	void LoadMesh(const aiMesh* aimesh, aiMaterial** const materials, jsh::Mesh* mesh, const char* path, const char* meshName)
	{
		mesh->SetRawData(jshGraphics::CreateRawData(meshName));
		mesh->SetMaterial(jshGraphics::CreateMaterial(meshName));
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
				textureCoords[i * 2u + 1u] = 1.f - aimesh->mTextureCoords[0][i].y;
			}
		}

		// MATERIAL
		const aiMaterial* material = materials[aimesh->mMaterialIndex];
		
		// properties
		uint32 cantProperties = material->mNumProperties;

		float shininess = 1.f;
		float specularIntensity = 1.f;
		int transparent = 0.f;

		aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);
		aiGetMaterialFloat(material, AI_MATKEY_SHININESS_STRENGTH, &specularIntensity);
		aiGetMaterialInteger(material, AI_MATKEY_BLEND_FUNC, &transparent);

		mesh->GetMaterial()->SetShininess(shininess);
		mesh->GetMaterial()->SetShininess(specularIntensity);
		
		//mesh->SetTransparent(transparent == aiBlendMode_Default);

		mesh->GetMaterial()->Create();

		// mapping
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString path0;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &path0);
			jsh::Texture* diffuseMap = jshGraphics::CreateTexture((std::string(meshName) + "[diffuse]").c_str());
			diffuseMap->type = JSH_TEXTURE_DIFFUSE_MAP;
			jshLoader::LoadTexture((std::string(path) + std::string(path0.C_Str())).c_str(), &diffuseMap->resource);
			mesh->SetTexture(diffuseMap);
		}
		if (material->GetTextureCount(aiTextureType_NORMALS) != 0) {
			aiString path0;
			material->GetTexture(aiTextureType_NORMALS, 0, &path0);
			jsh::Texture* normalMap = jshGraphics::CreateTexture((std::string(meshName) + "[normal]").c_str());
			normalMap->type = JSH_TEXTURE_NORMAL_MAP;
			jshLoader::LoadTexture((std::string(path) + std::string(path0.C_Str())).c_str(), &normalMap->resource);
			mesh->SetTexture(normalMap);
		}
		if (material->GetTextureCount(aiTextureType_SPECULAR) != 0) {
			aiString path0;
			material->GetTexture(aiTextureType_SPECULAR, 0, &path0);
			jsh::Texture* specularMap = jshGraphics::CreateTexture((std::string(meshName) + "[specular]").c_str());
			specularMap->type = JSH_TEXTURE_SPECULAR_MAP;
			jshLoader::LoadTexture((std::string(path) + std::string(path0.C_Str())).c_str(), &specularMap->resource);
			mesh->SetTexture(specularMap);
		}

		// MESH CREATION
		jsh::RawData* rawData = mesh->GetRawData();
		if (aimesh->HasTangentsAndBitangents()) {
			rawData->SetTangents((float*)aimesh->mTangents);
			rawData->SetBitangents((float*)aimesh->mBitangents);
		}
		if (textureCoords) mesh->GetRawData()->SetTextureCoords(textureCoords);
		rawData->SetIndices(iData, indexCount);
		rawData->SetPositionsAndNormals((float*)vertices, (float*)normals, aimesh->mNumVertices);
		rawData->Create();
		mesh->UpdatePrimitives();

		if (textureCoords) delete[] textureCoords;
	}

	void AddNode(aiNode* ainode, jsh::MeshNode* node, jsh::Mesh** meshes)
	{
		node->sons.resize(ainode->mNumChildren);
		XMMATRIX localMatrix = XMLoadFloat4x4((XMFLOAT4X4*)&ainode->mTransformation);

		// transformation
		jsh::vec3 position;
		jsh::vec3 rotation;
		jsh::vec3 scale;

		{
			XMVECTOR positionV;
			XMVECTOR qRotationV;
			XMVECTOR scaleV;
			
			XMMatrixDecompose(&scaleV, &qRotationV, &positionV, localMatrix);
			jsh::QuaternionToEuler((jsh::vec4*)& qRotationV, &rotation);

			XMStoreFloat3((XMFLOAT3*)& position, positionV);
			XMStoreFloat3((XMFLOAT3*)& scale, scaleV);
		}

		node->transform.SetPosition(position);
		node->transform.SetRotation(rotation);
		node->transform.SetScale(scale);
		ainode->mParent;
		
		if (ainode->mName.length > 0) node->name = ainode->mName.C_Str();
		if (ainode->mNumMeshes > 0) node->mesh = meshes[ainode->mMeshes[0]];

		// recursion
		for (uint32 i = 0; i < ainode->mNumChildren; ++i) {
			node->sons[i].mesh = nullptr;
			node->sons[i].transform = jsh::Transform();
			AddNode(ainode->mChildren[i], &node->sons[i], meshes);
		}
	}

	std::shared_ptr<jsh::Model> LoadModel(const char* absolutePath, const char* name)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(absolutePath, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);

		if (!scene || !scene->HasMeshes()) {
			jshLogE("Empty model %s", absolutePath);
			return false;
		}

		std::string pathStr = absolutePath;
		{
			uint32 pos = pathStr.size() - 1;
			while (pathStr[pos--] != '/') if (pos == 0) break;;
			if (pos != 0) {
				pathStr = pathStr.substr(0u, pos + 2);
			}
		}
		const char* path = pathStr.c_str();

		// MESHES
		uint32 numOfMeshes = scene->mNumMeshes;
		jsh::Mesh** meshes = new jsh::Mesh*[numOfMeshes];

		for (uint32 i = 0; i < numOfMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];

			if (!mesh->HasPositions() || !mesh->HasNormals()) {
				jshLogE("Invalid mesh, %s[%u]", path, i);
				continue;
			}

			// name
			std::stringstream meshName;
			meshName << name;
			if (numOfMeshes != 0) meshName << '[' << std::to_string(i) << ']';

			// mesh
			meshes[i] = jshGraphics::CreateMesh(meshName.str().c_str());
			LoadMesh(mesh, scene->mMaterials, meshes[i], path, meshName.str().c_str());
		}

		// NODES
		auto model = std::make_shared<jsh::Model>();

		aiNode* aiRoot = scene->mRootNode;
		if (aiRoot) {
			AddNode(aiRoot, &model->root, meshes);
		}

		return model;
	}

	bool LoadTexture(const char* path, jsh::Resource* texture)
	{
		int width, height, bits;
		byte* data = stbi_load(path, &width, &height, &bits, 4);
		if (!data) {
			jshLogE("Texture not found '%s'", path);
			return false;
		}

		uint32 pitch = width * 4u;

		JSH_TEXTURE2D_DESC desc;
		desc.ArraySize = 1u;
		desc.BindFlags = JSH_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0u;
		desc.Format = JSH_FORMAT_R8G8B8A8_UNORM;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1u;
		desc.MiscFlags = 0u;
		desc.SampleDesc.Count = 1u;
		desc.SampleDesc.Quality = 0u;
		desc.Usage = JSH_USAGE_DEFAULT;
		JSH_SUBRESOURCE_DATA sData;
		sData.pSysMem = data;
		sData.SysMemPitch = width * 4;

		jshGraphics::CreateResource(&desc, &sData, texture);

		stbi_image_free(data);

		return true;
	}

}