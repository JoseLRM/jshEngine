#pragma once

#include "GraphicsObjects.h"

namespace jsh {

	class SpriteRenderQueue {

		Shader* m_Shader;
		InputLayout m_InputLayout;

		struct Instance {
			Sprite sprite;
			Color color;
			XMMATRIX tm;
			float layer;

			Instance(const Sprite& s, const Color& color, const XMMATRIX& t, float layer) : sprite(s), color(color), tm(t), layer(layer) {}

			inline bool operator< (const Instance& other) const noexcept
			{
				if (layer < other.layer) return true;
				else return sprite.texture < other.sprite.texture;
			}
		};

		std::vector<Instance> m_Instances;

	public:

		void SetShader(Shader* shader) 
		{ 
			m_Shader = shader; 
			jshGraphics::GenerateInputLayout(m_Shader, jshGraphics::objects::GetSpriteRawData(), &m_InputLayout);
		}
		inline Shader* GetShader() noexcept { return m_Shader; }
		inline InputLayout GetInputLayout() noexcept { return m_InputLayout; }

		inline void Begin()
		{
			m_Instances.clear();
		}

		inline void Reserve(uint32 count)
		{
			m_Instances.reserve(count);
		}

		inline void DrawSprite(const Sprite& sprite, const Color& color, const XMMATRIX& tm, float layer)
		{
			m_Instances.emplace_back(sprite, color, tm, layer);
		}
		inline void DrawQuad(const vec3& position, vec2 size, float rotation, Color color, float layer) {
			XMMATRIX tm = XMMatrixTranspose(XMMatrixScaling(size.x, size.y, 0.f) * XMMatrixRotationZ(rotation) * XMMatrixTranslation(position.x, position.y, position.z));
			constexpr Sprite nullSprite;
			m_Instances.emplace_back(nullSprite, color, tm, layer);
		}
		inline void DrawQuad(const vec3& position, vec2 size, Color color, float layer) {
			XMMATRIX tm = XMMatrixTranspose(XMMatrixScaling(size.x, size.y, 0.f) * XMMatrixTranslation(position.x, position.y, position.z));
			constexpr Sprite nullSprite;
			m_Instances.emplace_back(nullSprite, color, tm, layer);
		}

		inline void End()
		{
			std::sort(m_Instances.begin(), m_Instances.end());
		}

		inline std::vector<Instance>& GetInstances() noexcept { return m_Instances; }

	};

}