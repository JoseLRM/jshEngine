#pragma once

#include "ConstantBuffer.h"

namespace jsh {

	class Mesh;
	struct Transform;
	struct Sprite;

	class Shader {
	protected:
		VertexShader	m_VS;
		PixelShader		m_PS;
		GeometryShader	m_GS;

		uint32 m_InstanceSize = 64u;

		jsh::vector<VertexProperty> m_VertexProperties;
		jsh::vector<VertexProperty> m_InstanceProperties;

#ifdef JSH_ENGINE
	public:
#endif
		Shader() = default;
		Shader(uint32 instanceSize) : m_InstanceSize(instanceSize) {}

	public:
		void Bind(jsh::CommandList cmd);

		// setters
		inline void SetVertexShader(const VertexShader& vs) noexcept { m_VS = vs; }
		inline void SetPixelShader(const PixelShader& ps) noexcept { m_PS = ps; }
		inline void SetGeometryShader(const GeometryShader& gs) noexcept { m_GS = gs; }

		inline void AddVertexProperty(const char* name, JSH_FORMAT format, uint32 index) noexcept
		{
			m_VertexProperties.push_back({ name, format, index }, 2);
		}
		inline void AddInstanceProperty(const char* name, JSH_FORMAT format, uint32 index) noexcept
		{
			m_InstanceProperties.push_back({ name, format, index }, 2);
		}

		// getters
		inline VertexShader& GetVertexShader() noexcept { return m_VS; }
		inline PixelShader& GetPixelShader() noexcept { return m_PS; }
		inline GeometryShader& GetGeometryShader() noexcept { return m_GS; }

		inline jsh::vector<VertexProperty>& GetVertexProperties() noexcept { return m_VertexProperties; }
		inline jsh::vector<VertexProperty>& GetInstanceProperties() noexcept { return m_InstanceProperties; }

		inline uint32 GetInstanceSize() const noexcept { return m_InstanceSize; }

		virtual ConstantData CreateConstantData() { return ConstantData(); }
		virtual void BindConstantData(ConstantData& cd, CommandList cmd) {}

		virtual void SetInstanceData(byte* data, Transform* trans);
		virtual void SetInstanceData(byte* data, const XMMATRIX& tm, const Sprite& sprite, const Color& color);

#ifdef JSH_IMGUI
		virtual void ShowConstantDataInfo(ConstantData& cd) {}
#endif

	};

	class SolidShader : public Shader {

	public:
		ConstantData CreateConstantData() override;
		void BindConstantData(ConstantData& cd, CommandList cmd) override;

		inline void SetShininess(float n, ConstantData& cd)
		{
			cd.constantBuffers[0].SetValue("Shininess", &n);
		}
		inline void SetSpecularIntensity(float n, ConstantData& cd)
		{
			cd.constantBuffers[0].SetValue("SpecularIntensity", &n);
		}

#ifdef JSH_IMGUI
		void ShowConstantDataInfo(ConstantData& cd) override;
#endif
	};

	class NormalShader : public Shader {

	public:
		ConstantData CreateConstantData() override;
		void BindConstantData(ConstantData& cd, CommandList cmd) override;

		inline void SetShininess(float n, ConstantData& cd)
		{
			cd.constantBuffers[0].SetValue("Shininess", &n);
		}
		inline void SetSpecularIntensity(float n, ConstantData& cd)
		{
			cd.constantBuffers[0].SetValue("SpecularIntensity", &n);
		}

		void SetDiffuseMap(Texture* texture, ConstantData& cd);
		void SetNormalMap(Texture* texture, ConstantData& cd);
		void SetSpecularMap(Texture* texture, ConstantData& cd);

#ifdef JSH_IMGUI
		void ShowConstantDataInfo(ConstantData& cd) override;
#endif
	};

}