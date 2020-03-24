#pragma once

#include "Entity.h"
#include "..//utils/dataStructures/vector.h"

namespace jsh {

	struct BaseComponent {
		ID_t entityID = 0;

	private:
		static ID_t s_IDcount;

	protected:
		static ID_t GetComponentID(size_t componentSize)
		{
			return s_IDcount++;
		}

	public:
		static ID_t GetComponentsCount()
		{
			return s_IDcount;
		}

	public:
		jshImGui(virtual void ShowInfo() {});

	};

	template<class T>
	struct Component : public BaseComponent {
		const static ID_t ID;
		const static size_t SIZE;
	};

	template<class T>
	const ID_t jsh::Component<T>::ID(BaseComponent::GetComponentID(sizeof(T)));
	template<class T>
	const size_t Component<T>::SIZE(sizeof(T));

}
#define jshDefineTag(name) struct name : public jsh::Component<name> {}; template class jsh::Component<name>
#define jshDefineComponent(name) template class jsh::Component<name>
