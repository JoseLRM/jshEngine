#include "common.h"
#include "Image.h"

namespace jsh {

	Image::Image() : m_Width(0u), m_Height(0u), m_Format(JSH_FORMAT_R32G32B32A32_FLOAT)
	{
		Clear();
	}
	Image::Image(uint32 size, JSH_FORMAT format) 
		: m_Width(size), m_Height(size), m_Format(format)
	{
		Clear();
		Allocate();
	}
	Image::Image(uint32 width, uint32 height, JSH_FORMAT format) 
		: m_Width(width), m_Height(height), m_Format(format)
	{
		Clear();
		Allocate();
	}
	Image::~Image()
	{
		Clear();
	}

	// copy
	Image::Image(const Image& other) noexcept
	{
		this->operator=(other);
	}
	Image& Image::operator=(const Image& other) noexcept
	{
		Clear();
		m_Width = other.GetWidth();
		m_Height = other.GetHeight();
		m_Format = other.GetFormat();
		Allocate();

		memcpy(m_Buffer, other.m_Buffer, Size());
		return *this;
	}
	// move
	Image::Image(Image&& other) noexcept
	{
		this->operator=(std::move(other));
	}
	Image& Image::operator=(Image&& other) noexcept
	{
		Clear();
		m_Width = other.GetWidth();
		m_Height = other.GetHeight();
		m_Format = other.GetFormat();
		m_Buffer = other.m_Buffer;

		other.m_Buffer = nullptr;
		other.m_Width = 0u;
		other.m_Height = 0u;
		return *this;
	}

	// setters
	void Image::Resize(uint32 size, JSH_FORMAT format)
	{
		Resize(size, size, format);
	}
	void Image::Resize(uint32 width, uint32 height, JSH_FORMAT format)
	{
		Clear();
		m_Width = width;
		m_Height = height;
		m_Format = format;
		Allocate();
	}

	void Image::Clear()
	{
		if (m_Buffer != nullptr) {
			delete[] m_Buffer;
			m_Buffer = nullptr;
		}
	}

	void Image::Allocate()
	{
		m_Buffer = new uint8[Size()];
	}

}