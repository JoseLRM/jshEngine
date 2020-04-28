#pragma once

#include "GraphicsPrimitives.h"

namespace jsh {

	class RawData;
	class Texture;
	class Material;
	class Shader;

	class Mesh {
		bool m_DiffuseMapEnabled = false;
		bool m_NormalMapEnabled = false;
		bool m_SpecularMapEnabled = false;

		Texture* m_DiffuseMap = nullptr;
		Texture* m_NormalMap = nullptr;
		Texture* m_SpecularMap = nullptr;

		RawData* m_RawData = nullptr;
		Material* m_Material = nullptr;
		Shader* m_Shader = nullptr;

		bool m_Transparent = false;

		bool m_Modified = true;

#ifdef JSH_ENGINE
	public:
#endif
		Mesh() = default;
	public:
		void Bind(jsh::CommandList cmd);

		void SetDiffuseMap(Texture* texture) noexcept;
		void SetNormalMap(Texture* texture) noexcept;
		void SetSpecularMap(Texture* texture) noexcept;

		void EnableDiffuseMap(bool enable) noexcept;
		void EnableNormalMap(bool enable) noexcept;
		void EnableSpecularMap(bool enable) noexcept;

		inline bool HasDiffuseMap() { return m_DiffuseMapEnabled; }
		inline bool HasNormalMap() { return m_NormalMapEnabled; }
		inline bool HasSpecularMap() { return m_SpecularMapEnabled; }

		void SetRawData(RawData* rawData) noexcept;
		void SetMaterial(Material* material) noexcept;
		void SetShader(Shader* shader) noexcept;
		inline void SetTransparent(bool t) noexcept { m_Transparent = t; }

		void UpdatePrimitives() noexcept;

		inline RawData* GetRawData() const noexcept { return m_RawData; }
		inline Material* GetMaterial() const noexcept { return m_Material; }
		inline Shader* GetShader() const noexcept { return m_Shader; }
		inline Texture* GetDiffuseMap() const noexcept { return m_DiffuseMap; }
		inline Texture* GetNormalMap() const noexcept { return m_NormalMap; }
		inline Texture* GetSpecularMap() const noexcept { return m_SpecularMap; }
		inline bool IsTransparent() const noexcept { return m_Transparent; }

	};


}