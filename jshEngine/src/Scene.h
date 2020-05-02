#pragma once

#include "System.h"
#include "vector.h"
#include "Debug.h"
#include "Timer.h"
#include <mutex>

#include "Components.h"

namespace jshScene {

	namespace _internal {

		jsh::vector<jsh::Entity>& GetEntitiesList();
		std::vector<jsh::EntityData>& GetEntityDataList();
		std::vector<std::vector<byte>>& GetComponentsList();

		jsh::Entity DuplicateEntity(jsh::Entity duplicate, jsh::Entity parent);

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

	inline jsh::Entity DuplicateEntity(jsh::Entity duplicated)
	{
		return _internal::DuplicateEntity(duplicated, _internal::GetEntityDataList()[duplicated].parent);
	}
	inline void SetLayer(jsh::Entity entity, jsh::Layer* layer)
	{
		_internal::GetEntityDataList()[entity].layer = layer;
	}
	inline jsh::Layer* GetLayer(jsh::Entity entity)
	{
		return _internal::GetEntityDataList()[entity].layer;
	}

	void GetEntitySons(jsh::Entity parent, jsh::Entity** sonsArray, uint32* size) noexcept;
	jsh::Entity GetEntityParent(jsh::Entity entity);
	jsh::Transform& GetTransform(jsh::Entity entity);
	jsh::Layer* GetLayerOf(jsh::Entity entity);

	void DestroyEntity(jsh::Entity entity) noexcept;

	void ClearScene();

	// system methods
	void UpdateSystem(jsh::System* system, float dt);
	void UpdateSystems(jsh::System** systems, uint32 cant, float dt);

	jsh::Time GetSystemPerformance(const jsh::System& system);

	// Layers
	void CreateLayer(const char* name, int16 value);
	jsh::Layer* GetLayer(const char* name);
	void DestroyLayer(const char* name);
	uint32 GetLayerCount();

	// ---------------DEBUG-------------------------
#if defined(JSH_IMGUI)
	namespace _internal {
		void ImGuiAddEntity(jsh::Entity entity);
	}
	bool ShowImGuiEntityWindow();
	bool ShowImGuiSystemsWindow();
#endif

}

#define JSH_DEFAULT_LAYER jshScene::GetLayer("Default")