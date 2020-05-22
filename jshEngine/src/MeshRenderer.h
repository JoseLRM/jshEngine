#pragma once

#include "common.h"

#include "Technique.h"

namespace jsh {

	struct MeshInstance {
		RawData* rawData;
		Material* material;
		XMFLOAT4X4 tm;

		MeshInstance(RawData* r, Material* m, const XMFLOAT4X4& tm) : rawData(r), material(m), tm(tm) {}

		inline bool operator< (const MeshInstance& other) const noexcept
		{
			if (material->GetShader() < other.material->GetShader()) return true;
			else return rawData < other.rawData;
		}
	};

	namespace _internal {

		class MeshRenderingMode {

		public:
			virtual void Initialize() = 0;
			virtual void Load(std::vector<MeshInstance>& instances, std::vector<uint32>& instancesIDs, std::vector<LightBufferData>& lights) = 0;
			virtual void Render(CommandList cmd, RenderTargetView& rtv, TextureRes& dsv, Viewport& vp, RenderGraph& rg) = 0;
			virtual void Close() = 0;

		};

		class ForwardRendering : public MeshRenderingMode {
			//TODO: static
			uint8 m_InstanceData[JSH_GFX_BATCH_COUNT * JSH_GFX_MESH_INSTANCE_SIZE];
			InstanceBuffer m_InstanceBuffer;

			Buffer m_LightBuffer;
			struct alignas(16) {
				LightBufferData lights[JSH_GFX_MAX_LIGHTS];
				vec4 ambient;
			} m_LightData;

			BlendState m_AddColorBlendState;

			std::vector<MeshInstance>* m_pInstances;
			std::vector<uint32>* m_pInstancesIDs;

		public:
			void Initialize() override;
			void Load(std::vector<MeshInstance>& instances, std::vector<uint32>& instancesIDs, std::vector<LightBufferData>& lights) override;
			void Render(CommandList cmd, RenderTargetView& rtv, TextureRes& dsv, Viewport& vp, RenderGraph& rg) override;
			void Close() override;

		};

	}

	class MeshRenderer : public Renderer {
		
		Viewport* m_pViewport;

		std::vector<MeshInstance>	m_Instances;
		std::vector<uint32>			m_InstancesID;

		/*
		0 -> Forward
		1 -> Deferred
		*/
		uint8 m_Mode = 0;

		std::unique_ptr<_internal::MeshRenderingMode> m_RenderingMode;

	public:
		MeshRenderer(RenderTargetView& rtv, TextureRes& dsv, Viewport& vp);

		void Initialize() override;
		void Update() override;
		void Render(CommandList cmd) override;
		void Close() override;

		void DrawScene(Camera& camera);

		void SetForwardRendering();
		void SetDeferredRendering();

	};

}