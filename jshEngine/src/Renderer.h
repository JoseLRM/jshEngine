#pragma once

#include "RenderGraph.h"

namespace jsh {

	class Renderer {
	protected:
		RenderGraph m_RenderGraph;

		Entity m_MainCamera = 0;

	public:
		inline void SetMainCamera(Entity cam) noexcept { m_MainCamera = cam; }
		inline Entity GetMainCamera() const noexcept { return m_MainCamera; }

		virtual bool Initialize() = 0;

		virtual void Begin() = 0;
		virtual void Render() = 0;
		virtual void End() = 0;

		virtual bool Close() = 0;

#ifdef JSH_IMGUI
		void ShowInfo() {}
#endif

	};

}