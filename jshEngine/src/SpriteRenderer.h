#pragma once

#include "common.h"

#include "Technique.h"

namespace jsh {

	struct SpriteInstance {
		Sprite sprite;
		Color4b color;
		XMMATRIX tm;
		float layer;

		SpriteInstance(Color4b color, const XMMATRIX& tm, float layer) : sprite(), color(color), tm(tm), layer(layer) {}
		SpriteInstance(const Sprite& s, const Color4b& color, const XMMATRIX& t, float layer) : sprite(s), color(color), tm(t), layer(layer) {}

		inline bool operator< (const SpriteInstance& other) const noexcept
		{
			if (layer != other.layer) return layer < other.layer;
			else return sprite.texture < other.sprite.texture;
		}
	};

	class SpriteRenderer : public Renderer {
		static VertexShader s_VertexShader;
		static PixelShader s_PixelShader;
		static InputLayout s_InputLayout;
		static Buffer s_VertexBuffer;
		static JSH_TOPOLOGY s_Topology;

		//TODO: static
		uint8 m_InstanceData[JSH_GFX_BATCH_COUNT * JSH_GFX_SPRITE_INSTANCE_SIZE];
		InstanceBuffer m_InstanceBuffer;

		std::vector<SpriteInstance> m_Instances;

		XMMATRIX m_ViewProjectionMatrix;
		
		InputLayout m_InputLayout;
		Viewport* m_pViewport;

	public:
		SpriteRenderer(RenderTargetView& rtv, Viewport& vp);

		void Initialize() override;
		void Update() override;
		void Render(CommandList cmd) override;
		void Close() override;

		void Begin(Camera& camera);
		void End();

		void DrawScene();

		void ReserveInstances(uint32 count);

		void DrawQuad(const vec2& position, const vec2& size, Color4b color, float layer);
		void DrawQuad(const vec2& position, const vec2& size, float rotation, Color4b color, float layer);
		void DrawQuad(const XMMATRIX& tm, Color4b color, float layer);

		void DrawSprite(const vec2& position, const vec2& size, const Sprite& sprite, Color4b color, float layer);
		void DrawSprite(const vec2& position, const vec2& size, float rotation, const Sprite& sprite, Color4b color, float layer);
		void DrawSprite(const XMMATRIX& tm, const Sprite& sprite, Color4b color, float layer);

	};

}