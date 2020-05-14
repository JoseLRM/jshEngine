#pragma once

#include "common.h"

#define JSH_ECS_SYSTEM_SAFE				0u
#define JSH_ECS_SYSTEM_PARALLEL			1u
#define JSH_ECS_SYSTEM_MULTITHREADED	2u

namespace jsh {

	class System {
	public:
		static uint32 s_SystemCount;

#ifdef JSH_ENGINE
	public:
#endif
		std::vector<uint16> m_RequestedComponents;
		std::vector<uint16> m_OptionalComponents;

		bool m_IndividualSystem = true;
		bool m_TreeMode = false;
		uint8 m_ExecuteType = JSH_ECS_SYSTEM_SAFE;

		std::string m_Name;
		uint32 m_SystemID;

	public:
		System() {
			m_SystemID = s_SystemCount++;
			m_Name = "System " + std::to_string(m_SystemID);
		}

		System(const char* name) {
			m_SystemID = s_SystemCount++;
			m_Name = name;
		}

		virtual void UpdateEntity(Entity entity, BaseComponent** components, float deltaTime) {}
		virtual void UpdateEntities(std::vector<BaseComponent**>& components, float deltaTime) {}

		inline bool IsIndividualSystem() const noexcept { return m_IndividualSystem; }
		inline bool IsCollectiveSystem() const noexcept { return !m_IndividualSystem; }

		inline void SetIndividualSystem() noexcept { m_IndividualSystem = true; }
		inline void SetCollectiveSystem() noexcept 
		{ 
			JSH_ASSERT(m_ExecuteType != JSH_ECS_SYSTEM_MULTITHREADED); 
			m_IndividualSystem = false; 
		}

		inline void SetTreeMode(bool treeMode) noexcept { m_TreeMode = treeMode; }
		inline bool InTreeMode() const noexcept { return m_TreeMode; }

		inline void SetExecuteType(uint8 type) noexcept
		{
			JSH_ASSERT(type <= 2 && IsCollectiveSystem() ? type != JSH_ECS_SYSTEM_MULTITHREADED : true);
			m_ExecuteType = type;
		}

		inline void SetName(const char* name) noexcept { m_Name = name; }
		inline const char* GetName() const noexcept { return m_Name.c_str(); }

		inline uint8 GetExecuteType() const noexcept
		{
			return m_ExecuteType;
		}

		inline uint32 GetSystemID() const noexcept { return m_SystemID; }

		inline std::vector<uint16>& GetRequestedComponents() noexcept { return m_RequestedComponents; }
		inline std::vector<uint16>& GetOptionalComponents() noexcept { return m_OptionalComponents; }

		inline void AddRequestedComponent(uint16 ID) noexcept
		{
			m_RequestedComponents.push_back(ID);
		}
		inline void AddOptionalComponents(uint16 ID) noexcept
		{
			m_OptionalComponents.push_back(ID);
		}

	};

}