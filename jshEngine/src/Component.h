#pragma once

#include "common.h"

namespace jsh {
	struct BaseComponent;
}

namespace jshScene {

	namespace _internal {
		uint16 GetComponentID();
		size_t SetComponentSize(uint16 ID, size_t size);
		const char* SetComponentNameAndConstructor(uint16 ID, const char* name, const jsh::BaseComponent* ptr);
	}

	uint16 GetComponentsCount();

	size_t GetComponentSize(uint16 ID);
	const char* GetComponentName(uint16 ID);
	void ConstructComponent(uint16 ID, jsh::BaseComponent* ptr);

}

namespace jsh {

	struct BaseComponent {
		uint32 entity = INVALID_ENTITY;

	public:
		virtual void Serialize(Archive& archive) {}
		virtual void Deserialize(Archive& archive) {}

#ifdef JSH_IMGUI
		virtual void ShowInfo() {}
#endif

	};

	template<typename T>
	struct Component : public BaseComponent {
		const static uint16 ID;
		const static size_t SIZE;
		const static const char* NAME;
		const static T CONSTRUCTOR;
	};

}

#pragma warning(disable : 4114)
#define jshDefineComponent(name) template struct jsh::Component<name>; \
const uint16 name::ID(jshScene::_internal::GetComponentID());\
const size_t name::SIZE(jshScene::_internal::SetComponentSize(name::ID, sizeof(name))); \
const name name::CONSTRUCTOR; \
const const char* name::NAME(jshScene::_internal::SetComponentNameAndConstructor(name::ID, #name, (jsh::BaseComponent*)&name::CONSTRUCTOR))

#define jshDefineTag(name) struct name : public jsh::Component<name> {}; jshDefineComponent(name)