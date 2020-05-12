#pragma once

#include "GraphicsDesc.h"

namespace jsh {

	class Image {
		uint32 m_Width;
		uint32 m_Height;

		byte* m_Buffer = nullptr;

		JSH_FORMAT m_Format;

	public:
		Image();
		Image(uint32 size, JSH_FORMAT format = JSH_FORMAT_R32G32B32A32_FLOAT);
		Image(uint32 width, uint32 height, JSH_FORMAT format = JSH_FORMAT_R32G32B32A32_FLOAT);
		~Image();

		// copy
		Image(const Image& other) noexcept;
		Image& operator=(const Image& other) noexcept;
		// move
		Image(Image&& other) noexcept;
		Image& operator=(Image&& other) noexcept;

		void Resize(uint32 size, JSH_FORMAT format = JSH_FORMAT_R32G32B32A32_FLOAT);
		void Resize(uint32 width, uint32 height, JSH_FORMAT format = JSH_FORMAT_R32G32B32A32_FLOAT);

		void Clear();

		// getters
		inline uint32 GetWidth() const noexcept { return m_Width; }
		inline uint32 GetHeight() const noexcept { return m_Height; }
		inline uvec2 GetDimension() const noexcept { return { m_Width, m_Height }; }

		inline byte* GetBuffer() const noexcept { return m_Buffer; }
		inline JSH_FORMAT GetFormat() const noexcept { return m_Format; }

		constexpr inline uint32 Size() const noexcept 
		{ 
			return m_Width * m_Height * jshGraphics::GetFormatStride(m_Format); 
		}

	private:
		void Allocate();

	};

}