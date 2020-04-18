#pragma once

#include "System.h"
#include "..//utils/dataStructures/vector.h"
#include "..//Debug.h"
#include "..//Timer.h"
#include <mutex>

#include "..//components/Components.h"

namespace jshScene {

	namespace _internal {

		jsh::vector<jsh::Entity>& GetEntitiesList();
		std::vector<jsh::EntityData>& GetEntityDataList();
		std::vector<std::vector<byte>>& GetComponentsList();

		void AddComponent(jsh::Entity entity, jsh::BaseComponent* comp, uint16 componentID, size_t componentSize) noexcept;
		void AddComponents(jsh::vector<jsh::Entity>& entities, jsh::BaseComponent* comp, uint16 componentID, size_t componentSize) noexcept;

		void RemoveComponent(jsh::Entity entity, uint16 componentID, size_t componentSize) noexcept;
		jsh::BaseComponent* GetComponent(jsh::Entity entity, uint16 componentID) noexcept;
		void RemoveComponents(jsh::EntityData& entityData) noexcept;

		template<typename Component>
		inline void AddComponents(jsh::vector<jsh::Entity>& entities, const Component& component)
		{
			_internal::AddComponents(entities, (jsh::BaseComponent*)& component, Component::ID, Component::SIZE);
		}
		template<typename Component, typename... Args>
		inline void AddComponents(jsh::vector<jsh::Entity>& entities, const Component& component, const Args& ... args)
		{
			_internal::AddComponents(entities, (jsh::BaseComponent*)& component, Component::ID, Component::SIZE);
			_internal::AddComponents(entities, args...);
		}

		// systems
		void UpdateIndividualSafeSystem(jsh::System* system, float deltaTime);
		void UpdateCollectiveAndMultithreadedSystem(jsh::System* system, float deltaTime);

		void SetUpdatePerformance(jsh::System* pSystem, jsh::Time beginTime) noexcept;
	}

	void Initialize() noexcept;
	void Close() noexcept;

	// component methods
	template<typename Component>
	inline void AddComponent(jsh::Entity entity, const Component& component) {
		_internal::AddComponent(entity, (jsh::BaseComponent*) & component, Component::ID, Component::SIZE);
	}
	template<typename Component, typename... Args>
	inline void AddComponent(jsh::Entity entity, const Component& component, const Args& ... args)
	{
		_internal::AddComponent(entity, (jsh::BaseComponent*) & component, Component::ID, Component::SIZE);
		AddComponent(entity, args...);
	}
	template<typename Component>
	inline void RemoveComponent(jsh::Entity entity) {
		_internal::RemoveComponent(entity, Component::ID, Component::SIZE);
	}
	template<typename Component, typename Component2, typename... Args>
	inline void RemoveComponent(jsh::Entity entity) {
		_internal::RemoveComponent(entity, Component::ID, Component::SIZE);
		RemoveComponent<Component2, Args...>(entity);
	}
	template<typename Component>
	inline Component* GetComponent(jsh::Entity entity)
	{
		return (Component*)_internal::GetComponent(entity, Component::ID);
	}

	// entity methods
	jsh::Entity CreateEntity() noexcept;
	template<typename Component, typename... Args>
	jsh::Entity CreateEntity(const Component& component, const Args& ... args) noexcept
	{
		jsh::Entity entity = CreateEntity();
		AddComponent(entity, component, args...);
		return entity;
	}

	void CreateEntities(uint32 cant, jsh::vector<jsh::Entity>* entities = nullptr) noexcept;
	template<typename Component, typename... Args>
	void CreateEntities(uint32 cant, jsh::vector<jsh::Entity>* entities, const Component& component, const Args& ... args) noexcept
	{
		bool notAlloc = !entities;
		if (notAlloc) entities = new jsh::vector<jsh::Entity>();
		CreateEntities(cant, entities);
		_internal::AddComponents(*entities, component, args...);
		if (notAlloc) delete entities;
	}

	jsh::Entity CreateSEntity(jsh::Entity parent) noexcept;
	template<typename Component, typename... Args>
	jsh::Entity CreateSEntity(jsh::Entity parent, const Component& component, const Args& ... args) noexcept
	{
		jsh::Entity entity = CreateSEntity(parent);
		AddComponent(entity, component, args...);
		return entity;
	}

	void CreateSEntities(jsh::Entity parent, uint32 cant, jsh::vector<jsh::Entity>* entities = nullptr) noexcept;
	template<typename Component, typename... Args>
	void CreateSEntities(jsh::Entity parent, uint32 cant, jsh::vector<jsh::Entity>* entities, const Component& component, const Args& ... args) noexcept
	{
		bool notAlloc = !entities;
		if (notAlloc) entities = new jsh::vector<jsh::Entity>();
		CreateSEntities(parent, cant, entities);
		_internal::AddComponents(*entities, component, args...);
		if (notAlloc) delete entities;
	}

	void GetEntitySons(jsh::Entity entity, jsh::vector<jsh::Entity>& entities) noexcept;
	jsh::Entity GetEntityParent(jsh::Entity entity);
	jsh::Transform& GetTransform(jsh::Entity entity);

	void DestroyEntity(jsh::Entity entity) noexcept;

	// system methods
	void UpdateSystem(jsh::System* system, float dt);
	void UpdateSystems(jsh::System** systems, uint32 cant, float dt);

	// ---------------DEBUG-------------------------
#if defined(JSH_IMGUI)
	namespace _internal {
		void ImGuiAddEntity(jsh::Entity entity);
	}
	bool ShowImGuiEntityWindow();
	bool ShowImGuiSystemsWindow();
#endif

}