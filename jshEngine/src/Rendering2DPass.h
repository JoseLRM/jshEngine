#pragma once

#include "RenderGraph.h"

namespace jsh {

	class SpriteBatch;

	class Rendering2DPass : public RenderPass
	{
		SpriteBatch* m_pSpriteBatch = nullptr;

		struct SpriteData {
			XMMATRIX tm;
			vec4 color;
			vec4 coords;
			uint16 textureID;
		};

		SpriteData m_SpriteData[JSH_GFX_SPRITE_BATCH_COUNT];
		Buffer m_SpriteInstances;

	public:

		inline void SetSpriteBatch(SpriteBatch* pSpriteBatch) noexcept { m_pSpriteBatch = pSpriteBatch; }

		void Create();
		void Load();
		void Render(CommandList cmd);

	};

}