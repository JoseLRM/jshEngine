#pragma once

#include "Entity.h"
#include "..//utils/dataStructures/vector.h"

namespace jsh {

	struct BaseComponent {
		uint32 entityID = 0;

	private:
		static uint16 s_IDcount;

	protected:
		static uint16 GetComponentID(size_t componentSize)
		{
			return s_IDcount++;
		}

	public:
		static uint16 GetComponentsCount()
		{
			return s_IDcount;
		}

	public:
		jshImGui(virtual void ShowInfo() {});

	};

	template<class T>
	struct Component : public BaseComponent {
		const static uint16 ID;
		const static size_t SIZE;
	};

	template<class T>
	const uint16 jsh::Component<T>::ID(BaseComponent::GetComponentID(sizeof(T)));
	template<class T>
	const size_t Component<T>::SIZE(sizeof(T));

}
#define jshDefineTag(name) struct name : public jsh::Component<name> {}; template class jsh::Component<name>
#define jshDefineComponent(name) template class jsh::Component<name>

// core components
namespace jsh {

	struct NameComponent : jsh::Component<NameComponent> {
		const char* name;

		NameComponent(const char* name) : name(name) {}
	};
	jshDefineComponent(NameComponent);

}