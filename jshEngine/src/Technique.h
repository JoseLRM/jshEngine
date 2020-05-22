#pragma once

#include "common.h"
#include "RenderGraph.h"

namespace jsh {

	class Technique {
		bool m_Rendering = false;
		GfxPrimitive* m_Attachments[JSH_GFX_ATTACHMENTS_COUNT];
		RenderGraph* m_pRenderGraph = nullptr;

	protected:
		uint32 AddAttachment(GfxPrimitive* att) noexcept;
		inline GfxPrimitive* GetAttachment(uint32 i) const noexcept
		{
			JSH_ASSERT(i < JSH_GFX_ATTACHMENTS_COUNT);
			return m_Attachments[i];
		}
		RenderGraph& GetRenderGraph() const noexcept { return *m_pRenderGraph; }

	public:
		friend RenderGraph;

		void Load();
		void Run();
		inline bool IsRendering() const { return m_Rendering; }

		virtual void Initialize() = 0;
		virtual void Close() = 0;

		bool HasAttachment(GfxPrimitive* att) const noexcept;

	protected:
		virtual void Update() = 0;
		virtual void Render(CommandList cmd) = 0;

	};

}