#pragma once

#include "Graphics.h"
#include "vector.h"
#include "CameraComponent.h"

namespace jsh {

	struct SpriteData {
		XMMATRIX tm;
		float layer;
		Color color;
		jsh::vec4 texCoords;
		Texture* pTexture;

		inline bool operator<(const SpriteData& other) {
			if (layer < other.layer) return true;
			if (layer > other.layer) return false;
			if (layer == other.layer) {
				return pTexture < other.pTexture;
			}
		}
	};

	struct SpriteInstance {
		XMMATRIX tm;
		Color color;
		vec4 coords;
		uint16 textureID;
	};

	class SpriteBatch {

		jsh::vector<SpriteData> m_Batch;

		uint32 m_Offset = 0u;

	public:
		SpriteBatch() {
			m_Batch.reserve(JSH_GFX_SPRITE_BATCH_COUNT);
		}
		
		//////////////////////////////////////DRAW METHODS/////////////////////////////////////////////

		// from matrix
		inline void Draw(const XMMATRIX& tm, const Color& color, const Sprite& sprite, float layer)
		{
			m_Batch.push_back({ XMMatrixTranspose(tm), layer, color, sprite.coords, sprite.texture }, JSH_GFX_SPRITE_BATCH_COUNT);
		}

		// from vector
		inline void Draw(const vec2& position, const vec2& size, float rotation, const Color& color, const Sprite& sprite, float layer)
		{
			XMMATRIX tm = XMMatrixScaling(size.x, size.y, 1.f) * XMMatrixRotationZ(rotation) * XMMatrixTranslation(position.x, position.y, 0.f);
			Draw(tm, color, sprite, layer);
		}

		////////////////////////////////////////////////////////////////////////////////////
		inline void Begin() noexcept
		{
			m_Batch.clear(false);
		}

		inline void End()
		{
			// sort pools
			std::sort(m_Batch.data(), m_Batch.data() + m_Batch.size());
			m_Offset = 0u;
		}

		inline void FillPoolAndBindTextures(size_t* size, SpriteInstance* data, CommandList cmd)
		{			
			if (m_Batch.size() <= m_Offset) {
				*size = 0u;
				return;
			}

			SpriteData* pool = m_Batch.data() + m_Offset;
			*size = m_Batch.size() - m_Offset;
			if (*size > JSH_GFX_SPRITE_BATCH_COUNT)* size = JSH_GFX_SPRITE_BATCH_COUNT;

			// Bind and load textures
			Texture* textures[JSH_GFX_TEXTURES_COUNT];
			jshZeroMemory(textures, sizeof(Texture*) * JSH_GFX_TEXTURES_COUNT);
			uint16 texCount = 0u;

			for (uint32 i = 0; i < *size; ++i) {
				SpriteData& d = pool[i];
				SpriteInstance& instance = data[i];

				instance.textureID = 0u;

				if (d.pTexture != nullptr) {
					for (uint8 j = 0; j < texCount; ++j) {
						if (textures[j] == d.pTexture) {
							instance.textureID = j + 1;
							break;
						}
					}

					if (instance.textureID == 0u) {
						if (texCount == JSH_GFX_TEXTURES_COUNT) {
							*size = i;
							break;
						}
						textures[texCount++] = d.pTexture;
						instance.textureID = texCount;
					}
				}

				instance.tm = d.tm;
				instance.color = d.color;
				instance.coords = d.texCoords;
			}

			for (uint32 i = 0; i < texCount; ++i) {
				jshGraphics::BindTexture(textures[i]->resource, i, JSH_SHADER_TYPE_PIXEL, cmd);
				jshGraphics::BindSamplerState(textures[i]->samplerState, i, JSH_SHADER_TYPE_PIXEL, cmd);
			}

			m_Offset += *size;
		}

	};

}