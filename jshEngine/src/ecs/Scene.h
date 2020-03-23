#pragma once

#include "System.h"
#include "..//utils/dataStructures/vector.h"
#include "..//Debug.h"
namespace jsh {

	class Scene {
		jsh::vector<Entity> m_Entities;
		std::vector<EntityData> m_EntityData;
		jsh::vector<Entity> m_FreeEntityData;

		static jsh::vector<size_t> s_ComponentsSize;

		jsh::vector<jsh::vector<byte>> m_Components;

		// internal methods
	private:
		void AddComponent(Entity entity, BaseComponent* comp, ID_t componentID, size_t componentSize) noexcept;
		void AddComponents(jsh::vector<Entity>& entities, BaseComponent* comp, ID_t componentID, size_t componentSize) noexcept;
		
		void RemoveComponent(Entity entity, ID_t componentID, size_t componentSize) noexcept;
		inline BaseComponent* GetComponent(const EntityData& entity, ID_t componentID) noexcept
		{
			auto it = entity.m_Indices.find(componentID);
			if (it != entity.m_Indices.end()) return (BaseComponent*)(&m_Components[componentID][it->second]);

			return nullptr;
		}		
		void RemoveComponents(EntityData& entityData) noexcept;

		template<typename Component>
		void AddComponents(jsh::vector<Entity>& entities, const Component& component)
		{
			AddComponents(entities, (BaseComponent*)& component, Component::ID, Component::SIZE);
		}
		template<typename Component, typename... Args>
		void AddComponents(jsh::vector<Entity>& entities, const Component& component, const Args& ... args)
		{
			AddComponents(entities, (BaseComponent*)&component, Component::ID, Component::SIZE);
			AddComponents(entities, args...);
		}

		void UpdateIndividualSafeSystem(System* system, float deltaTime);
		void UpdateCollectiveAndMultithreadedSystem(System* system, float deltaTime);

	public:
		Scene();
		void Create() noexcept;
		~Scene();

		// entity methods
		Entity CreateEntity() noexcept;
		template<typename Component, typename... Args>
		Entity CreateEntity(const Component& component, const Args&... args) noexcept
		{
			Entity entity = CreateEntity();
			AddComponent(entity, component, args...);
			return entity;
		}

		void CreateEntities(uint32 cant, jsh::vector<Entity>* entities = nullptr) noexcept;
		template<typename Component, typename... Args>
		void CreateEntities(uint32 cant, jsh::vector<Entity>* entities, const Component& component, const Args& ... args) noexcept
		{
			bool notAlloc = !entities;
			if (notAlloc) entities = new jsh::vector<Entity>();
			CreateEntities(cant, entities);
			AddComponents(*entities, component, args...);
			if (notAlloc) delete entities;
		}

		Entity CreateSEntity(Entity parent) noexcept;
		template<typename Component, typename... Args>
		Entity CreateSEntity(Entity parent, const Component& component, const Args& ... args) noexcept
		{
			Entity entity = CreateSEntity(parent);
			AddComponent(entity, component, args...);
			return entity;
		}

		void CreateSEntities(Entity parent, uint32 cant, jsh::vector<Entity>* entities = nullptr) noexcept;
		template<typename Component, typename... Args>
		void CreateSEntities(Entity parent, uint32 cant, jsh::vector<Entity>* entities, const Component& component, const Args& ... args) noexcept
		{
			bool notAlloc = !entities;
			if (notAlloc) entities = new jsh::vector<Entity>();
			CreateSEntities(parent, cant, entities);
			AddComponents(*entities, component, args...);
			if (notAlloc) delete entities;
		}

		inline void GetEntitySons(Entity entity, jsh::vector<Entity>& entities) noexcept
		{
			const EntityData& parent = m_EntityData[entity];
			entities.clear();
			entities.resize(parent.sonsCount);
			for (uint32 i = 0; i < parent.sonsCount; ++i) {
				entities.push_back_nr(m_Entities[parent.handleIndex + i + 1]);
			}
		}
		inline Entity GetEntityParent(Entity entity) {
			return m_EntityData[entity].parent;
		}

		void DestroyEntity(Entity entity) noexcept;

		// component methods
		template<typename Component>
		inline void AddComponent(Entity entity, const Component& component) {
			AddComponent(entity, (BaseComponent*) &component, Component::ID, Component::SIZE);
		}
		template<typename Component, typename... Args>
		inline void AddComponent(Entity entity, const Component& component, const Args& ... args)
		{
			AddComponent(entity, (BaseComponent*)& component, Component::ID, Component::SIZE);
			AddComponent(entity, args...);
		}
		template<typename Component>
		inline void RemoveComponent(Entity entity) {
			RemoveComponent(entity, Component::ID, Component::SIZE);
		}
		template<typename Component, typename Component2, typename... Args>
		inline void RemoveComponent(Entity entity) {
			RemoveComponent(entity, Component::ID, Component::SIZE);
			RemoveComponent<Component2, Args...>(entity);
		}
		template<typename Component>
		inline Component* GetComponent(Entity entity)
		{
			return (Component*)GetComponent(m_EntityData[entity], Component::ID);
		}

		// system methods
		void UpdateSystem(System* system, float dt);
		void UpdateSystems(System** systems, uint32 cant, float dt);

		// Debug
		void PrintEntities() {
			for (uint32 i = 1; i < m_Entities.size(); ++i) {
				jshLogln("Entity -> %u", m_Entities[i]);
			}
		}

		void PrintComponent(ID_t compID)
		{
			auto& list = m_Components[compID];
			size_t compSize = s_ComponentsSize[compID];
			for (size_t i = 0; i < list.size(); i += compSize) {
				jshLogln("Component[%u] %u -> %u", compID, i, ((BaseComponent*)(&list[i]))->entityID);
			}
		}

	};

}