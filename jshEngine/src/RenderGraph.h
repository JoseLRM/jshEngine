#pragma once

#include "Graphics.h"

namespace jsh {

	struct CameraComponent;
	class RenderGraph;

	class RenderPass {
	protected:
		RenderGraph* m_pRenderGraph = nullptr;
		jsh::vector<RenderPass*> m_Depencences;

	public:
		friend RenderGraph;
		RenderPass() {}

		virtual void Create() = 0;
		virtual void Load() {}
		virtual void Render(CommandList cmd) = 0;

		void AddDependence(RenderPass* renderPass) noexcept;

	};

	class RenderGraph {
		jsh::vector<RenderPass*> m_RenderPasses;

		bool m_Modified = true;

		struct {
			XMMATRIX vm;

			jsh::vec4 position;
			jsh::vec4 aux0;
			jsh::vec4 aux1;
			jsh::vec4 aux2;
		} m_CameraBufferData;

		struct Light {
			vec4 lightPos;
			vec4 data;
			vec4 color;
			vec4 direction;
		};
		struct alignas(16) {
			Light lights[JSH_GFX_MAX_LIGHTS];
		} m_LightBufferData;

		Buffer m_CameraBuffer;
		Buffer m_LightBuffer;

		CameraComponent* m_pCurrentCamera = nullptr;

	public:
		RenderGraph();

		inline Buffer& GetCameraBuffer() noexcept { return m_CameraBuffer; }
		inline Buffer& GetLightBuffer() noexcept { return m_LightBuffer; }

		inline CameraComponent* GetCurrentCamera() const noexcept { return m_pCurrentCamera; }

		void Create();
		void Render(CameraComponent* camera);

		inline void AddPass(RenderPass* pass) noexcept 
		{ 
			m_RenderPasses.push_back(pass);
			m_Modified = true;
			pass->m_pRenderGraph = this;
		}

	private:
		void UpdateGraph();
	};

}