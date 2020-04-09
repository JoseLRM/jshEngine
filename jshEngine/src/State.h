#pragma once

#include "common.h"
#include "ecs/Scene.h"

namespace jsh {

	class State {
	public:
		State() = default;
		virtual ~State() = default;

		virtual void Initialize() {}

		virtual void Update(float deltaTime) {}
		virtual void FixedUpdate() {}

		virtual void Render() {}

		virtual void Close() {}
		
#if defined(JSH_IMGUI)
	private:
		uint16 m_ShowIndex = 0u;
#ifdef JSH_ENGINE
	public:
#endif
		bool ShowImGuiWindow()
		{
			// TODO: TEMPORAL
			bool result = true;
			if (ImGui::Begin("State", 0, ImGuiWindowFlags_MenuBar)) {

				if (ImGui::BeginMenuBar()) {

					if (ImGui::Button("Entities")) {
						m_ShowIndex = 1;
					}
					if (ImGui::Button("Systems")) {
						m_ShowIndex = 2;
					}

					ImGui::EndMenuBar();
				}
				switch (m_ShowIndex)
				{
				case 1:
					jshScene::ShowEntityWindow();
					break;
				case 2:
					jshScene::ShowSystemsWindow();
					break;
				}

				if (ImGui::Button("Close")) result = false;

			}ImGui::End();
			return result;
		}
#endif
	};

}