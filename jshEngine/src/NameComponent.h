#pragma once

#include "Scene.h"

namespace jsh {

	struct NameComponent : jsh::Component<NameComponent> {
		char name[JSH_ECS_NAMECOMPONENT_SIZE];

		void SetName(const char* n) noexcept {
			jshZeroMemory(name, JSH_ECS_NAMECOMPONENT_SIZE);
			for (uint32 i = 0; i < JSH_ECS_NAMECOMPONENT_SIZE; ++i) {
				if (n[i] == '\0') break;
				name[i] = n[i];
			}
		}

		void SetName(const std::string& str) noexcept {
			jshZeroMemory(name, JSH_ECS_NAMECOMPONENT_SIZE);
			for (size_t i = 0; i < str.size(); ++i) {
				if (i >= JSH_ECS_NAMECOMPONENT_SIZE) break;
				name[i] = str[i];
			}
		}

		NameComponent() {
			SetName("Unnamed");
		}
		NameComponent(const NameComponent& other) {
			SetName(other.name);
		}
		NameComponent(const char* n) {
			SetName(n);
		}
		NameComponent(const std::string& str) {
			SetName(str);
		}

		NameComponent& operator=(const NameComponent& other) {
			SetName(other.name);
			return *this;
		}
		

#ifdef JSH_IMGUI
		void ShowInfo() override
		{
			ImGui::InputText("Name", name, 32);
		}
#endif

	};
	jshDefineComponent(NameComponent);

}