#include "Exception.h"

#include "Debug.h"

namespace jsh {

	Exception::Exception() : m_Type(), m_Description() {}
	Exception::Exception(const wchar* type) : m_Type(type), m_Description() {}
	Exception::Exception(const wchar* type, const wchar* desc) : m_Type(type), m_Description(desc) {}

	const wchar* Exception::what() const
	{
		jshDebug::ShowOkWindow(m_Type.c_str(), m_Description.c_str(), 3);

		return m_Type.c_str();
	}

}