#pragma once

#include <exception>
#include "common.h"

namespace jsh {

	class Exception {
		std::wstring m_Type;
		std::wstring m_Description;

	public:
		Exception();
		Exception(const wchar* type);
		Exception(const wchar* type, const wchar* desc);

		const wchar* what() const;

		inline void SetType(const wchar* type) noexcept { m_Type = type; }
		inline void SetDescription(const wchar* desc) noexcept { m_Description = desc; }

		inline const wchar* GetType() const noexcept { return m_Type.c_str(); }
		inline const wchar* GetDescription() const noexcept { return m_Description.c_str(); }

	};

}

#define jshException(type, desc) jsh::Exception(type, desc)