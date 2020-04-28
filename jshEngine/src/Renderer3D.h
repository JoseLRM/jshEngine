#pragma once

#include "Renderer.h"

#include "Rendering3DPass.h"
#include "OutlineRenderPass.h"
#include "PostProcessRenderPass.h"
#include "MeshBatch.h"

namespace jsh {

	class Renderer3D : public Renderer {
		Rendering3DPass		m_Rendering3DPass;
		OutlineRenderPass		m_OutlineRenderPass;
		PostProcessRenderPass	m_PostProcessRenderPass;

		MeshBatch m_MeshBatch;

	public:
		bool Initialize() override;

		void Begin() override;
		void Render() override;
		void End() override;
		
		bool Close() override;

	};

}