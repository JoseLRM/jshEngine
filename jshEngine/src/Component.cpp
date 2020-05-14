#include "common.h"

using namespace jsh;

namespace jshScene {
	uint16 g_IDcount = 0;

	struct ComponentData {
		const char* name;
		size_t size;
		const jsh::BaseComponent* constructor;
	};

	ComponentData g_ComponentData[JSH_ECS_MAX_COMPONENTS_TYPES];

	namespace _internal {
		uint16 GetComponentID() {

			if (g_IDcount == JSH_ECS_MAX_COMPONENTS_TYPES) {
				jshDebug::_internal::LogF("You exceeded components types count, just change 'JSH_ECS_MAX_COMPONENTS_TYPES' in 'config.h'", __LINE__, __FILE__);
				exit(1);
			}

			return g_IDcount++;
		}
		size_t SetComponentSize(uint16 ID, size_t size)
		{
			g_ComponentData[ID].size = size;
			return size;
		}
		const char* SetComponentNameAndConstructor(uint16 ID, const char* name, const jsh::BaseComponent* ptr)
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
	void ConstructComponent(uint16 ID, jsh::BaseComponent* ptr)
	{
		memcpy(ptr, g_ComponentData[ID].constructor, g_ComponentData[ID].size);
	}

}