#pragma once

#include "GraphicsPrimitives.h"
#include "Material.h"

namespace jsh {

	class RawData;
	class Material;
	class Shader;

	class Mesh {
		RawData* m_RawData = nullptr;
		Material* m_Material = nullptr;
		InputLayout m_InputLayout;

		bool m_Valid = false;

#ifdef JSH_ENGINE
	public:
#endif
		Mesh() = default;
	public:
		void Draw(uint32 instances, jsh::CommandList cmd);

		void SetRawData(RawData* rawData) noexcept;
		void SetMaterial(Material* material) noexcept;
		void UpdateInputLayout() noexcept;

		inline RawData* GetRawData() const noexcept { return m_RawData; }
		inline Material* GetMaterial() const noexcept { return m_Material; }
		inline Shader* GetShader() const noexcept { return m_Material->GetShader(); }
		inline InputLayout GetInputLayout() const noexcept { return m_InputLayout; }

		inline bool IsValid() const noexcept { return m_Valid; }

	};


}