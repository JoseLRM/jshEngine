#include "common.h"

#include "Technique.h"

namespace jsh {

	uint32 Technique::AddAttachment(GfxPrimitive* att) noexcept
	{
		for (uint32 i = 0; i < JSH_GFX_ATTACHMENTS_COUNT; ++i) {
			if (m_Attachments[i] == nullptr) {
				m_Attachments[i] = att;
				return i;
			}
		}
		jshDebug::LogE("You are exceeded the limit of attachments, change 'JSH_GFX_ATTACHMENTS_COUNT' in config.h");
		return 0;
	}

	void Technique::Load()
	{
		m_Rendering = true;
		Update();
	}

	void Technique::Run()
	{
		CommandList cmd = jshGraphics::BeginCommandList();

		//jshTask::Execute([this, cmd]() {
		
			Render(cmd);
			m_Rendering = false;

		//});
	}

	bool Technique::HasAttachment(GfxPrimitive* att) const noexcept
	{
		for (uint32 i = 0; i < JSH_GFX_ATTACHMENTS_COUNT; ++i) {
			if (m_Attachments[i] == att) return true;
		}
		return false;
	}

}