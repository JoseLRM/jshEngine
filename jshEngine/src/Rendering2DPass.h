#pragma once

#include "RenderGraph.h"
#include "SpriteBatch.h"

namespace jsh {

	class Rendering2DPass : public RenderPass
	{
		SpriteBatch* m_pSpriteBatch = nullptr;

		SpriteInstance m_SpriteData[JSH_GFX_SPRITE_BATCH_COUNT];
		Buffer m_SpriteInstances;

	public:

		inline void SetSpriteBatch(SpriteBatch* pSpriteBatch) noexcept { m_pSpriteBatch = pSpriteBatch; }

		void Create();
		void Load();
		void Render(CommandList cmd);

	};

}