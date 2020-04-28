#pragma once

#include "Graphics.h"
#include "batch_pool.h"
#include "CameraComponent.h"



namespace jsh {

	struct SpriteInstance {
		XMMATRIX tm;
		vec4 color;
		jsh::vec4 texCoords;
		Texture* pTexture;
		uint16 textureID = 0u;

		inline bool operator<(const SpriteInstance& other) {
			return pTexture < other.pTexture;
		}
	};

	class SpriteBatch {

		std::vector<batch_pool<SpriteInstance>> m_Batch;

		uint32 m_Layer = 0u;
		uint32 m_Pool = 0u;
		uint32 m_Offset = 0u;

	public:
		SpriteBatch() {}
		SpriteBatch(uint32 instances) {}
		
		//////////////////////////////////////DRAW METHODS/////////////////////////////////////////////

		// from matrix
		inline void Draw(const XMMATRIX& tm, const vec4& color, const Sprite& sprite, jsh::Layer* layer)
		{
			m_Batch[layer->ID].push({ XMMatrixTranspose(tm), color, sprite.coords, sprite.texture });
		}

		// from vector
		inline void Draw(const vec2& position, const vec2& size, float rotation, const vec4& color, const Sprite& sprite, jsh::Layer* layer)
		{
			XMMATRIX tm = XMMatrixScaling(size.x, size.y, 1.f) * XMMatrixRotationZ(rotation) * XMMatrixTranslation(position.x, position.y, 0.f);
			Draw(tm, color, sprite, layer);
		}

		////////////////////////////////////////////////////////////////////////////////////
		inline void Begin() noexcept
		{
			uint32 minInstances = JSH_GFX_SPRITE_BATCH_COUNT / 1000;

			uint32 numOfLayers = jshScene::GetLayerCount();
			if (m_Batch.size() < numOfLayers) {
				uint32 cant = numOfLayers - m_Batch.size();
				m_Batch.reserve(cant);
				for (uint32 i = 0; i < cant; ++i) {
					m_Batch.emplace_back(JSH_GFX_SPRITE_BATCH_COUNT);
				}
			}

			for (uint32 i = 0; i < numOfLayers; ++i) {
				batch_pool<SpriteInstance>& batch = m_Batch[i];
				uint32 newCapacity = 0u;

				if (batch.pool_count() > 1 && batch.size() != JSH_GFX_SPRITE_BATCH_COUNT) {
					newCapacity = batch.size();
					newCapacity *= batch.pool_count() - 1u;
					newCapacity += batch.get_pool_size(batch.pool_count() - 1);
					{
						float countf = newCapacity;
						float minInstancesf = minInstances;

						newCapacity = (uint32)((float)((int32)(countf / minInstances) + 1) * minInstancesf);
					}
					if (newCapacity >= JSH_GFX_SPRITE_BATCH_COUNT) newCapacity = JSH_GFX_SPRITE_BATCH_COUNT;
				}
				m_Batch[i].reset(newCapacity);
			}
		}

		inline void End()
		{
			// sort pools
			for (uint32 i = 0; i < m_Batch.size(); ++i) {
				for (uint32 j = 0; j < m_Batch[i].pool_count(); ++j) {
					SpriteInstance* ptr = m_Batch[i].get_pool(j);
					size_t size = m_Batch[i].get_pool_size(j);

					std::sort(ptr, ptr + size);
				}
			}

			m_Layer = 0u;
			m_Pool = 0u;
			m_Offset = 0u;
		}

		inline SpriteInstance* GetPoolAndBindTextures(size_t* size, CommandList cmd)
		{
			if (m_Layer >= m_Batch.size()) {
				*size = 0u;
				return nullptr;
			}

			batch_pool<SpriteInstance>& batch = m_Batch[m_Layer];
			
			SpriteInstance* pool = batch.get_pool(m_Pool) + m_Offset;
			*size = batch.get_pool_size(m_Pool) - m_Offset;

			uint32 spritesWithTexturesIndex = 0u;
			while (pool[spritesWithTexturesIndex++].pTexture == nullptr && spritesWithTexturesIndex < *size);

			bool nextPool = true;

			Texture* currentTexture = nullptr;
			uint16 texCount = 0u;

			for (uint32 i = spritesWithTexturesIndex; i < *size; ++i) {
				if (pool[i].pTexture != currentTexture) {
					if (++texCount == JSH_GFX_TEXTURES_COUNT) {
						
						m_Offset += i;
						*size = i;
						nextPool = false;
						break;

					}
					currentTexture = pool[i].pTexture;
					jshGraphics::BindTexture(currentTexture->resource, texCount - 1, JSH_SHADER_TYPE_PIXEL, cmd);
					jshGraphics::BindSamplerState(currentTexture->samplerState, texCount - 1, JSH_SHADER_TYPE_PIXEL, cmd);
				}
				pool[i].textureID = texCount;
			}

			if (nextPool) {
				m_Offset = 0u;
				m_Pool++;
				if (batch.pool_count() == m_Pool) m_Layer++;
			}

			return pool;
		}

	};

}