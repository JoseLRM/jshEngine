#pragma once

#include "GraphicsPrimitives.h"
#include <vector>
#include "vector.h"
#include <map>

namespace jsh {

	class ConstantBuffer {

		struct Value {
			uint32 size;
			uint32 offset;
			Value() : size(0), offset(0) {}
			Value(uint32 size, uint32 offset) : size(size), offset(offset) {}
		};

		std::vector<byte> m_Data;
		std::map<const char*, Value> m_Layout;
		uint32 m_Size = 0u;

		Buffer m_Buffer;

		bool m_Created = false;
		bool m_Modified = true;

	public:
		void Create(uint32 CPUAccessFlags = 0u, JSH_USAGE usage = JSH_USAGE_DEFAULT, uint32 miscFlags = 0u);

		Buffer& Get(CommandList cmd) noexcept 
		{ 
			assert(m_Created && "You must create the constant buffer");
			if (m_Modified)
			{
				jshGraphics::UpdateBuffer(m_Buffer, m_Data.data(), m_Data.size(), cmd);
				m_Modified = false;
			}
			return m_Buffer; 
		}

		inline uint32 Size() const noexcept {
			return m_Size;
		}

		inline byte* data() noexcept { return m_Data.data(); }

		inline void Update() noexcept { m_Modified = true; }

		void Reset() noexcept;

		void AddValue(const char* name, void* value, uint32 size);
		void SetValue(const char* name, void* value);
		void* GetValue(const char* name);

	};

	class Texture;

	struct ConstantData {
		std::vector<ConstantBuffer> constantBuffers;
		TextureRes textures[JSH_GFX_TEXTURES_COUNT];
		SamplerState samplers[JSH_GFX_SAMPLER_STATES_COUNT];
	};

}