#pragma once

#include "System.h"
#include "..//utils/dataStructures/vector.h"
#include "..//Debug.h"
#include "..//Timer.h"
#include <mutex>

#include "..//ImGui/imgui.h"

namespace jsh {

	class Scene {
		jsh::vector<Entity> m_Entities;
		std::vector<EntityData> m_EntityData;
		jsh::vector<Entity> m_FreeEntityData;

		static jsh::vector<size_t> s_ComponentsSize;
		jsh::vector<jsh::vector<byte>> m_Components;

		std::map<ID_t, System*> m_UpdatedSystems;
		std::mutex m_UpdatedSystemsMutex;

		// internal methods
	private:
		void AddComponent(Entity entity, BaseComponent* comp, ID_t componentID, size_t componentSize) noexcept;
		void AddComponents(jsh::vector<Entity>& entities, BaseComponent* comp, ID_t componentID, size_t componentSize) noexcept;
		
		void RemoveComponent(Entity entity, ID_t componentID, size_t componentSize) noexcept;
		inline BaseComponent* GetComponent(const EntityData& entity, ID_t componentID) noexcept
		{
			auto it = entity.m_Indices.find(componentID);
			if (it != entity.m_Indices.end()) return (BaseComponent*)(&(m_Components[componentID][it->second]));

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

		// systems
		void UpdateIndividualSafeSystem(System* system, float deltaTime);
		void UpdateCollectiveAndMultithreadedSystem(System* system, float deltaTime);

		inline void SetUpdatePerformance(System* pSystem, jsh::Time beginTime) noexcept
		{
			pSystem->m_TimeCount += pSystem->m_LastTime;
			pSystem->m_UpdatesCount++;
			pSystem->m_LastTime = jshTimer::Now() - beginTime;

			m_UpdatedSystemsMutex.lock();
			m_UpdatedSystems[pSystem->m_SystemID] = pSystem;
			m_UpdatedSystemsMutex.unlock();
		}

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

		// ---------------DEBUG-------------------------
#if defined(JSH_IMGUI) && defined(JSH_ENGINE)
	private:
		Entity m_SelectedEntity = INVALID_ENTITY;

		void ImGuiAddEntity(Entity entity)
		{
			EntityData& entityData = m_EntityData[entity];

			bool empty = entityData.sonsCount == 0;
			auto treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | (empty ? ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_AllowItemOverlap);

			bool active = ImGui::TreeNodeEx(("Entity " + std::to_string(entity)).c_str(), treeFlags);
			if (ImGui::IsItemClicked()) m_SelectedEntity = entity;
			if (active) {
				if (!empty) {
					for (uint32 i = 0; i < entityData.sonsCount; ++i) {
						Entity e = m_Entities[entityData.handleIndex + i + 1];
						i += m_EntityData[e].sonsCount;
						ImGuiAddEntity(e);
					}
					ImGui::TreePop();
				}
				else ImGui::TreePop();
			}
		}
	public:
		void ShowEntityWindow()
		{
			ImGui::Columns(2);

			for (size_t i = 1; i < m_Entities.size(); ++i) {

				Entity entity = m_Entities[i];
				EntityData& entityData = m_EntityData[entity];

				if (entityData.parent == INVALID_ENTITY) {
					ImGuiAddEntity(entity);
					i += entityData.sonsCount;
				}

			}

			ImGui::NextColumn();

			if (m_SelectedEntity != INVALID_ENTITY) {

				ImGui::Text(("Entity " + std::to_string(m_SelectedEntity)).c_str());

				EntityData& entityData = m_EntityData[m_SelectedEntity];

				for (auto& it : entityData.m_Indices) {

					ID_t compID = it.first;
					size_t index = it.second;

					BaseComponent* comp = (BaseComponent*)(&(m_Components[compID][index]));
					comp->ShowInfo();

				}
			}

			ImGui::NextColumn();

		}

		void ShowSystemsWindow()
		{

			for (uint32 i = 0; i < m_UpdatedSystems.size(); ++i) {
				System* system = m_UpdatedSystems[i];

				ImGui::Text(("[" + std::string(system->GetName()) + "] -> " + 
					std::to_string(system->m_TimeCount / system->m_UpdatesCount)).c_str());

			}

		}

#endif

	};

}