#include "Component.h"

#include "Debug.h"

using namespace jsh;

namespace jshScene {
	uint16 g_IDcount = 0;

	struct ComponentData {
		const char* name;
		size_t size;
		const BaseComponent* constructor;
	};

	ComponentData g_ComponentData[JSH_ECS_MAX_COMPONENTS_TYPES];

	namespace _internal {
		uint16 GetComponentID() {

			if (g_IDcount == JSH_ECS_MAX_COMPONENTS_TYPES) {
				jshDebug::ShowOkWindow(L"You exceeded components types count, just change 'JSH_ECS_MAX_COMPONENTS_TYPES' in 'config.h'", 3);
				exit(1);
			}

			return g_IDcount++;
		}
		size_t SetComponentSize(uint16 ID, size_t size)
		{
			g_ComponentData[ID].size = size;
			return size;
		}
		const char* SetComponentNameAndConstructor(uint16 ID, const char* name, const BaseComponent* ptr)
		{
			g_ComponentData[ID].name = name;
			g_ComponentData[ID].constructor = ptr;
			return name;
		}
	}

	uint16 GetComponentsCount()
	{
		return g_IDcount;
	}
	size_t GetComponentSize(uint16 ID)
	{
		return g_ComponentData[ID].size;
	}
	const char* GetComponentName(uint16 ID)
	{
		return g_ComponentData[ID].name;
	}
	void ConstructComponent(uint16 ID, BaseComponent* ptr)
	{
		memcpy(ptr, g_ComponentData[ID].constructor, g_ComponentData[ID].size);
	}

}