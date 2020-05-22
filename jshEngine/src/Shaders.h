#pragma once

#include "GraphicsObjects.h"

namespace jsh {

	class SolidShader : public MeshShader {

		struct MaterialData {

			Buffer buffer;
			bool update = false;

			struct {
				float shininess = 1.f;
				float specIntensity = 1.f;
				vec2 _padding0;

				Color3f color = jshColor3f::WHITE;
				float _padding1;
			} mat;

		};

	public:
		void Create() override;

		void* CreateMaterialData() override;
		void DestroyMaterialData(void* matData) override;
		void BindMaterialData(void* matData, CommandList cmd) override;

		void SetShininess(float n, void* matData);
		void SetSpecularIntensity(float n, void* matData);

#ifdef JSH_IMGUI
		void ShowMaterialInfo(void* matData) override;
#endif
	};

	class NormalShader : public MeshShader {

		struct MaterialData {

			Buffer buffer;

			TextureRes* diffuseMap	= nullptr;
			TextureRes* normalMap	= nullptr;
			TextureRes* specularMap = nullptr;

			SamplerState* diffuseSampler = nullptr;
			SamplerState* sampler	= nullptr;

			bool update = false;
			struct {
				float shininess = 1.f;
				float specIntensity = 1.f;
				BOOL hasDiffuse = FALSE;
				BOOL hasNormal = FALSE;

				BOOL hasSpecular = FALSE;
				vec3 _padding0;
			} mat;

		};

	public:
		void Create() override;

		void* CreateMaterialData() override;
		void DestroyMaterialData(void* matData) override;
		void BindMaterialData(void* matData, CommandList cmd) override;

		void SetShininess(float n, void* matData);
		void SetSpecularIntensity(float n, void* matData);

		void SetDiffuseMap(Texture* texture, void* matData);
		void SetNormalMap(Texture* texture, void* matData);
		void SetSpecularMap(Texture* texture, void* matData);

#ifdef JSH_IMGUI
		void ShowMaterialInfo(void* matData) override;
#endif
	};

}