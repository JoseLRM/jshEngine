#include "Scene.h"
#include "..//TaskSystem.h"

using namespace jsh;
uint32 jsh::System::s_SystemCount = 0;

namespace jshScene {

	using namespace _internal;

	jsh::vector<Entity> g_Entities;
	std::vector<EntityData> g_EntityData;
	jsh::vector<Entity> g_FreeEntityData;

	jsh::vector<size_t> g_ComponentsSize;
	jsh::vector<jsh::vector<byte>> g_Components;

	std::map<uint16, System*> g_UpdatedSystems;
	std::mutex g_UpdatedSystemsMutex;

	namespace _internal {
		jsh::vector<jsh::Entity>& GetEntitiesList()
		{
			return g_Entities;
		}
		std::vector<jsh::EntityData>& GetEntityDataList()
		{
			return g_EntityData;
		}
		jsh::vector<jsh::vector<byte>>& GetComponentsList()
		{
			return g_Components;
		}

		BaseComponent* GetComponent(jsh::Entity e, uint16 componentID) noexcept
		{
			jsh::EntityData& entity = g_EntityData[e];
			auto it = entity.m_Indices.find(componentID);
			if (it != entity.m_Indices.end()) return (BaseComponent*)(&(g_Components[componentID][it->second]));

			return nullptr;
		}
		BaseComponent* GetComponent(const jsh::EntityData& entity, uint16 componentID) noexcept
		{
			auto it = entity.m_Indices.find(componentID);
			if (it != entity.m_Indices.end()) return (BaseComponent*)(&(g_Components[componentID][it->second]));

			return nullptr;
		}
		void SetUpdatePerformance(jsh::System* pSystem, jsh::Time beginTime) noexcept
		{
			pSystem->m_TimeCount += pSystem->m_LastTime;
			pSystem->m_UpdatesCount++;
			pSystem->m_LastTime = jshTimer::Now() - beginTime;

			g_UpdatedSystemsMutex.lock();
			g_UpdatedSystems[pSystem->m_SystemID] = pSystem;
			g_UpdatedSystemsMutex.unlock();
		}
	}

	void Initialize() noexcept
	{
		for (uint16 id = 0; id < BaseComponent::GetComponentsCount(); ++id) {
			g_Components.push_back(jsh::vector<byte>());
		}
		CreateEntity(); // allocate invalid Entity :)

		if (g_ComponentsSize.empty()) {
			g_ComponentsSize.resize(BaseComponent::GetComponentsCount());
		}
	}

	void Close() noexcept
	{
		for (uint16 id = 0; id < g_Components.size(); ++id) {
			g_Components[id].clear();
		}
		g_Components.clear();
		g_Entities.clear();
		g_EntityData.clear();
	}

	//////////////////////////////////ENTITIES////////////////////////////////
	Entity CreateEntity() noexcept
	{
		Entity entity;
		if (g_FreeEntityData.size() != 0) {
			entity = g_FreeEntityData.back();
			g_FreeEntityData.pop_back();

			EntityData& entityData = g_EntityData[entity];
			entityData.handleIndex = g_Entities.size();
		}
		else {
			entity = Entity(g_EntityData.size());

			EntityData entityData;
			entityData.handleIndex = g_Entities.size();

			if (g_EntityData.capacity() == g_EntityData.size()) g_EntityData.reserve(10u);
			g_EntityData.push_back(entityData);
		}
		
		g_Entities.push_back(entity, 10u);
		return entity;
	}
	void CreateEntities(uint32 cant, jsh::vector<Entity>* entities) noexcept
	{
		if (entities) entities->reserve(cant);

		size_t entityIndex = g_Entities.size();
		g_Entities.resize(g_Entities.size() + cant);
		while (!g_FreeEntityData.empty() && cant > 0) {
			Entity entity = g_FreeEntityData.back();
			g_FreeEntityData.pop_back();
			cant--;

			EntityData& entityData = g_EntityData[entity];
			entityData.handleIndex = entityIndex++;

			g_Entities.push_back_nr(entity);
			if (entities) entities->push_back_nr(entity);
		}

		g_EntityData.reserve(cant);
		Entity beginEntity = Entity(g_EntityData.size());
		for (uint32 i = 0; i < cant; ++i) {
			EntityData ed;
			ed.handleIndex = entityIndex + i;
			g_EntityData.push_back(ed);
			Entity entity = beginEntity + i;
			g_Entities.push_back(entity);
			if (entities) entities->push_back_nr(entity);
		}
	}

	Entity CreateSEntity(Entity parent) noexcept
	{
		EntityData& parentData = g_EntityData[parent];

		// update parents count
		jsh::vector<Entity> parentsToUpdate;
		parentsToUpdate.push_back(parent, 3);
		while (!parentsToUpdate.empty()) {

			Entity e = parentsToUpdate.back();
			parentsToUpdate.pop_back();
			EntityData& eData = g_EntityData[e];
			eData.sonsCount++;

			if (eData.parent != INVALID_ENTITY) parentsToUpdate.push_back(eData.parent, 3);
		}

		Entity entity;
		if (g_FreeEntityData.size() != 0) {
			entity = g_FreeEntityData.back();
			g_FreeEntityData.pop_back();
		}
		else {
			entity = Entity(g_EntityData.size());

			if (g_EntityData.capacity() == g_EntityData.size()) g_EntityData.reserve(10u);
			g_EntityData.push_back(EntityData());
		}
		
		EntityData& entityData = g_EntityData[entity];
		entityData.parent = parent;
		size_t index = parentData.handleIndex + parentData.sonsCount;
		entityData.handleIndex = index;

		// special case, the parent and sons are in back of the list
		if (index == g_Entities.size()) {
			g_Entities.push_back(entity, 10u);
			return entity;
		}

		if (g_Entities.reserve_request()) {
			g_Entities.reserve(10u);
		}

		g_Entities.insert(index, entity);

		for (size_t i = index + 1; i < g_EntityData.size(); ++i) {
			g_EntityData[i].handleIndex++;
		}

		return entity;
	}
	
	void CreateSEntities(Entity parent, uint32 cant, jsh::vector<Entity>* entities) noexcept
	{
		// update parents count
		jsh::vector<Entity> parentsToUpdate;
		parentsToUpdate.push_back(parent, 3);
		while (!parentsToUpdate.empty()) {

			Entity e = parentsToUpdate.back();
			parentsToUpdate.pop_back();
			EntityData& eData = g_EntityData[e];
			eData.sonsCount += cant;

			if (eData.parent != INVALID_ENTITY) parentsToUpdate.push_back(eData.parent, 3);
		}

		// reserve memory
		g_Entities.reserve(cant);

		EntityData& parentData = g_EntityData[parent];
		size_t entityIndex = size_t(parent) + size_t(parentData.sonsCount) - size_t(cant) + 1u;

		// if the parent and sons aren't in back of the list
		if (entityIndex != g_Entities.size()) {
			size_t newDataIndex = size_t(parent) + size_t(parentData.sonsCount) + 1u;
			// move old entities
			for (size_t i = g_Entities.capacity() - 1; i >= newDataIndex; --i) {
				g_Entities[i] = g_Entities[i - cant];
				g_EntityData[g_Entities[i]].handleIndex = i;
			}
		}

		// creating entities
		if (entities) entities->reserve(cant);
		while (!g_FreeEntityData.empty() && cant > 0) {
			Entity entity = g_FreeEntityData.back();
			g_FreeEntityData.pop_back();
			cant--;

			EntityData& entityData = g_EntityData[entity];
			entityData.handleIndex = entityIndex++;
			entityData.parent = parent;
			g_Entities[entityData.handleIndex] = entity;
			if (entities) entities->push_back_nr(entity);
		}

		Entity firstEntity = Entity(g_EntityData.size());
		g_EntityData.reserve(cant);
		for (uint32 i = 0; i < cant; ++i) {

			Entity entity = firstEntity + i;
			EntityData ed;
			ed.handleIndex = entityIndex + i;
			ed.parent = parent;
			g_EntityData.push_back(ed);
			g_Entities[ed.handleIndex] = entity;
			if (entities) entities->push_back_nr(entity);
		}
		g_Entities.add_pos(cant);
	}

	void DestroyEntity(Entity entity) noexcept
	{
		EntityData& entityData = g_EntityData[entity];
		uint32 cant = entityData.sonsCount + 1;
		
		// notify parents
		if (entityData.parent != INVALID_ENTITY) {
			jsh::vector<Entity> updateInheritance;
			updateInheritance.push_back(entityData.parent);
			while (!updateInheritance.empty()) {

				Entity p = updateInheritance.back();
				updateInheritance.pop_back();

				EntityData& pData = g_EntityData[entity];
				pData.sonsCount -= cant;

				if (pData.parent != INVALID_ENTITY) updateInheritance.push_back(pData.parent, 3);
				
			}
		}

		// data to remove entities
		size_t indexBeginDest = entityData.handleIndex;
		size_t indexBeginSrc = entityData.handleIndex + cant;
		size_t cpyCant = g_Entities.size() - indexBeginSrc;

		// remove components & entityData
		g_FreeEntityData.reserve(cant);
		for (uint32 i = 0; i < cant; i++) {
			Entity e = g_Entities[indexBeginDest + i];
			EntityData& ed = g_EntityData[e];
			RemoveComponents(ed);
			ed.handleIndex = 0;
			ed.parent = INVALID_ENTITY;
			g_FreeEntityData.push_back_nr(e);
		}

		// remove from entities & update indices
		memcpy(&g_Entities[indexBeginDest], &g_Entities[indexBeginSrc], cpyCant * sizeof(Entity));
		g_Entities.sub_pos(cant);
		for (size_t i = indexBeginDest; i < g_Entities.size(); ++i) {
			g_EntityData[g_Entities[i]].handleIndex = i;
		}
		
	}

	void GetEntitySons(Entity entity, jsh::vector<Entity>& entities) noexcept
	{
		const EntityData& parent = g_EntityData[entity];
		entities.clear();
		entities.resize(parent.sonsCount);
		for (uint32 i = 0; i < parent.sonsCount; ++i) {
			entities.push_back_nr(g_Entities[parent.handleIndex + i + 1]);
		}
	}

	jsh::Entity GetEntityParent(jsh::Entity entity) {
		return g_EntityData[entity].parent;
	}

	////////////////////////////////COMPONENTS////////////////////////////////
	namespace _internal {
		void AddComponent(Entity entity, BaseComponent* comp, uint16 componentID, size_t componentSize) noexcept
		{
			comp->entityID = entity;
			auto& list = g_Components[componentID];
			size_t index = list.size();

			// register the size
			if (list.empty()) g_ComponentsSize[componentID] = componentSize;

			// allocate the component
			if (list.capacity() < index + componentSize) {
				list.reserve(componentSize * 5);
			}
			list.add_pos(componentSize);
			memcpy(&list[index], comp, componentSize);

			// set index in entity
			g_EntityData[entity].m_Indices[componentID] = index;
		}

		void AddComponents(jsh::vector<Entity>& entities, BaseComponent* comp, uint16 componentID, size_t componentSize) noexcept
		{
			auto& list = g_Components[componentID];
			size_t index = list.size();

			// register the size
			if (list.empty()) g_ComponentsSize[componentID] = componentSize;

			// allocate the components
			list.reserve(componentSize * entities.size());
			list.add_pos(componentSize * entities.size());

			size_t currentIndex;
			Entity currentEntity;
			for (uint32 i = 0; i < entities.size(); ++i) {
				currentIndex = index + (i * componentSize);
				currentEntity = entities[i];

				memcpy(&list[currentIndex], comp, componentSize);
				// set entity in component
				BaseComponent* component = (BaseComponent*)(&list[currentIndex]);
				component->entityID = currentEntity;
				// set index in entity
				g_EntityData[currentEntity].m_Indices[componentID] = currentIndex;
			}
		}

		void RemoveComponent(Entity entity, uint16 componentID, size_t componentSize) noexcept
		{
			EntityData& entityData = g_EntityData[entity];
			auto it = entityData.m_Indices.find(componentID);
			if (it == entityData.m_Indices.end()) return;

			size_t index = (*it).second;
			entityData.m_Indices.erase(it);

			auto& list = g_Components[componentID];

			// if the component isn't the last element
			if (index != list.size() - componentSize) {
				// set back data in index
				memcpy(&list[index], &list[list.size() - componentSize], componentSize);

				Entity otherEntity = ((BaseComponent*)(&list[index]))->entityID;
				g_EntityData[otherEntity].m_Indices[componentID] = index;
			}

			list.sub_pos(componentSize);
		}

		void RemoveComponents(EntityData& entityData) noexcept
		{
			uint16 componentID;
			size_t componentSize, index;
			for (auto& it : entityData.m_Indices) {
				componentID = it.first;
				componentSize = g_ComponentsSize[componentID];
				index = it.second;
				auto& list = g_Components[componentID];

				if (index != list.size() - componentSize) {
					// set back data in index
					memcpy(&list[index], &list[list.size() - componentSize], componentSize);

					Entity otherEntity = ((BaseComponent*)(&list[index]))->entityID;
					g_EntityData[otherEntity].m_Indices[componentID] = index;
				}

				list.sub_pos(componentSize);
			}
			entityData.m_Indices.clear();
		}
	}

	////////////////////////////////SYSTEMS////////////////////////////////

	void UpdateSystem(System* system, float dt)
	{
		jsh::Time beginTime = jshTimer::Now();

		if(system->GetExecuteType() == JSH_ECS_SYSTEM_SAFE 
			|| system->GetExecuteType() == JSH_ECS_SYSTEM_PARALLEL) {

			if (system->IsIndividualSystem()) UpdateIndividualSafeSystem(system, dt);
			else UpdateCollectiveAndMultithreadedSystem(system, dt);
			
		}
		else {
			if (system->IsIndividualSystem()) UpdateCollectiveAndMultithreadedSystem(system, dt);
		}
		SetUpdatePerformance(system, beginTime);
	}

	void UpdateSystems(System** systems, uint32 cant, float dt)
	{
		System* pSystem;

		// update safe systems
		for (uint32 i = 0; i < cant; ++i) {
			pSystem = systems[i];
			if (pSystem->GetExecuteType() == JSH_ECS_SYSTEM_SAFE) {

				jsh::Time beginTime = jshTimer::Now();

				if (pSystem->IsIndividualSystem()) UpdateIndividualSafeSystem(pSystem, dt);
				else UpdateCollectiveAndMultithreadedSystem(pSystem, dt);

				SetUpdatePerformance(pSystem, beginTime);
			}
		}

		// update multithreaded systems
		for (uint32 i = 0; i < cant; ++i) {
			pSystem = systems[i];
			if (pSystem->GetExecuteType() == JSH_ECS_SYSTEM_MULTITHREADED) {

				jsh::Time beginTime = jshTimer::Now();

				if (pSystem->IsIndividualSystem()) UpdateCollectiveAndMultithreadedSystem(pSystem, dt);

				SetUpdatePerformance(pSystem, beginTime);
			}
		}

		// update parallel systems
		for (uint32 i = 0; i < cant; ++i) {
			pSystem = systems[i];
			if (pSystem->GetExecuteType() == JSH_ECS_SYSTEM_PARALLEL) {
				if (pSystem->IsIndividualSystem()) 
					jshTask::Execute([pSystem, dt]() { 

					jsh::Time beginTime = jshTimer::Now();
					UpdateIndividualSafeSystem(pSystem, dt);
					SetUpdatePerformance(pSystem, beginTime);

				});
				else jshTask::Execute([pSystem, dt]() { 

					jsh::Time beginTime = jshTimer::Now();
					UpdateCollectiveAndMultithreadedSystem(pSystem, dt); 
					SetUpdatePerformance(pSystem, beginTime);

				});
			}
		}

		jshTask::Wait();
	}

	namespace _internal {
		void UpdateIndividualSafeSystem(System* system, float dt)
		{
			// system requisites
			auto& request = system->GetRequestedComponents();
			auto& optional = system->GetOptionalComponents();
			uint32 cantOfComponents = Entity(request.size() + optional.size());

			if (request.size() == 0) {
				jshLogW("System '%s' haven't requested components", system->GetName());
				return;
			}

			// for optimization, choose the sortest component list
			uint32 indexOfBestList = 0;
			uint16 idOfBestList = request[0];
			for (uint32 i = 1; i < request.size(); ++i) {
				if (g_Components[request[i]].size() < g_Components[idOfBestList].size()) {
					idOfBestList = request[i];
					indexOfBestList = i;
				}
			}
			// if one request is empty, exit
			auto& list = g_Components[idOfBestList];
			if (list.size() == 0) return;
			size_t sizeOfBestList = g_ComponentsSize[idOfBestList];

			// reserve memory for the pointers
			jsh::vector<BaseComponent*> components;
			components.reserve(cantOfComponents);
			components.add_pos(cantOfComponents);

			// for all the entities
			BaseComponent* compOfBestList;
			if (system->InTreeMode()) { // entities sorted by inheritance
				for (size_t i = 0; i < g_Entities.size(); ++i) {
					Entity entity = g_Entities[i];
					compOfBestList = GetComponent(g_EntityData[entity], idOfBestList);

					bool isValid = compOfBestList;
					if (isValid) {
						EntityData& entityData = g_EntityData[entity];
						// allocate the best component
						components[indexOfBestList] = compOfBestList;

						// allocate requested components
						uint32 j;
						for (j = 0; i < request.size(); ++j) {
							if (j == indexOfBestList) continue;

							BaseComponent* comp = GetComponent(entityData, request[j]);
							if (!comp) {
								isValid = false;
								break;
							}
							components[j] = comp;
						}

						if (!isValid) {
							i += g_EntityData[entity].sonsCount;
							continue;
						}
						// allocate optional components
						for (j = 0; j < optional.size(); ++j) {

							BaseComponent* comp = GetComponent(entityData, optional[j]);
							components[j + request.size()] = comp;
						}

						// if the entity is valid, call update
						system->UpdateEntity(entity, components.data(), dt);
					}
				}
			}
			else { // normal
				for (size_t i = 0; i < list.size(); i += sizeOfBestList) {

					// allocate the best component
					compOfBestList = (BaseComponent*)(&list[i]);
					components[indexOfBestList] = compOfBestList;

					// entity
					Entity entity = compOfBestList->entityID;
					EntityData& entityData = g_EntityData[entity];
					bool isValid = true;

					// allocate requested components
					uint32 j;
					for (j = 0; j < request.size(); ++j) {
						if (j == indexOfBestList) continue;

						BaseComponent* comp = GetComponent(entityData, request[j]);
						if (!comp) {
							isValid = false;
							break;
						}
						components[j] = comp;
					}

					if (!isValid) continue;
					// allocate optional components
					for (j = 0; j < optional.size(); ++j) {

						BaseComponent* comp = GetComponent(entityData, optional[j]);
						components[j + request.size()] = comp;
					}

					// if the entity is valid, call update
					system->UpdateEntity(entity, components.data(), dt);
				}
			}
		}

		void UpdateCollectiveAndMultithreadedSystem(System* system, float dt)
		{
			jsh::vector<BaseComponent**> componentsList;
			// system requisites
			auto& request = system->GetRequestedComponents();
			auto& optional = system->GetOptionalComponents();
			uint32 cantOfComponents = uint32(request.size() + optional.size());

			if (request.size() == 0) {
				jshLogW("System '%s' haven't requested components", system->GetName());
				return;
			}

			// for optimization, choose the sortest component list
			uint32 indexOfBestList = 0;
			uint16 idOfBestList = request[0];
			for (uint32 i = 1; i < request.size(); ++i) {
				if (g_Components[request[i]].size() < g_Components[idOfBestList].size()) {
					idOfBestList = request[i];
					indexOfBestList = i;
				}
			}
			// if one request is empty, exit
			auto& list = g_Components[idOfBestList];
			if (list.size() == 0) return;
			size_t sizeOfBestList = g_ComponentsSize[idOfBestList];

			// for all the entities
			BaseComponent* compOfBestList;
			BaseComponent** components = new BaseComponent * [cantOfComponents];

			if (system->InTreeMode() && system->IsCollectiveSystem()) {
				for (size_t i = 0; i < g_Entities.size(); ++i) {

					Entity entity = g_Entities[i];
					compOfBestList = GetComponent(g_EntityData[entity], idOfBestList);

					bool isValid = compOfBestList;
					if (isValid) {
						EntityData& entityData = g_EntityData[entity];
						components[indexOfBestList] = compOfBestList;
						uint32 j;
						for (j = 0; j < request.size(); ++j) {
							if (j == indexOfBestList) continue;

							BaseComponent* comp = GetComponent(entityData, request[j]);
							if (!comp) {
								isValid = false;
								break;
							}
							components[j] = comp;
						}

						if (!isValid) {
							i += entityData.sonsCount;
							continue;
						}

						for (j = 0; j < optional.size(); ++j) {
							BaseComponent* comp = GetComponent(entityData, optional[j]);
							components[j + request.size()] = comp;
						}

						componentsList.push_back(components, 50);
						components = new BaseComponent * [cantOfComponents];
					}
				}
			}
			else {
				for (size_t i = 0; i < list.size(); i += sizeOfBestList) {

					compOfBestList = (BaseComponent*)(&list[i]);
					components[indexOfBestList] = compOfBestList;
					EntityData& entityData = g_EntityData[compOfBestList->entityID];

					bool isValid = true;

					uint32 j;
					for (j = 0; j < request.size(); ++j) {
						if (j == indexOfBestList) continue;

						BaseComponent* comp = GetComponent(entityData, request[j]);

						if (!comp) {
							isValid = false;
							break;
						}

						components[j] = comp;
					}

					if (!isValid) continue;

					for (j = 0; j < optional.size(); ++j) {
						BaseComponent* comp = GetComponent(entityData, optional[j]);
						components[j + request.size()] = comp;
					}

					componentsList.push_back(components, 50);
					components = new BaseComponent * [cantOfComponents];

				}
			}

			if (system->IsIndividualSystem()) {
				jshTask::Async(componentsList.size(), jshTask::ThreadCount(), [&componentsList, system, dt](ThreadArgs args) {

					system->UpdateEntity(componentsList[args.index][0]->entityID, componentsList[args.index], dt);

				});

				jshTask::Wait();
			}
			else {
				system->UpdateEntities(componentsList, dt);
			}

			// free memory
			for (size_t i = 0; i < componentsList.size(); ++i) {
				delete componentsList[i];
			}
		}
	}

	//////////////////////////////////DEBUG////////////////////////////////

#if defined(JSH_IMGUI)
	jsh::Entity m_SelectedEntity = INVALID_ENTITY;

	namespace _internal {
		void ImGuiAddEntity(jsh::Entity entity)
		{
			EntityData& entityData = g_EntityData[entity];

			bool empty = entityData.sonsCount == 0;
			auto treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | (empty ? ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_AllowItemOverlap);

			bool active;
			NameComponent* nameComponent = (NameComponent*)GetComponent(entity, NameComponent::ID);
			if (nameComponent) {
				active = ImGui::TreeNodeEx(nameComponent->name, treeFlags);
			}
			else {
				active = ImGui::TreeNodeEx(("Entity " + std::to_string(entity)).c_str(), treeFlags);
			}

			if (ImGui::IsItemClicked()) m_SelectedEntity = entity;
			if (active) {
				if (!empty) {
					for (uint32 i = 0; i < entityData.sonsCount; ++i) {
						Entity e = g_Entities[entityData.handleIndex + i + 1];
						i += g_EntityData[e].sonsCount;
						ImGuiAddEntity(e);
					}
					ImGui::TreePop();
				}
				else ImGui::TreePop();
			}
		}
	}

	bool ShowImGuiEntityWindow()
	{
		bool result = true;
		if (ImGui::Begin("Entities")) {
			ImGui::Columns(2);

			for (size_t i = 1; i < g_Entities.size(); ++i) {

				Entity entity = g_Entities[i];
				EntityData& entityData = g_EntityData[entity];

				if (entityData.parent == INVALID_ENTITY) {
					ImGuiAddEntity(entity);
					i += entityData.sonsCount;
				}

			}

			if (m_SelectedEntity < 0 || m_SelectedEntity >= g_Entities.size()) m_SelectedEntity = INVALID_ENTITY;

			ImGui::NextColumn();

			if (m_SelectedEntity != INVALID_ENTITY) {

				NameComponent* nameComponent = (NameComponent*)GetComponent(m_SelectedEntity, NameComponent::ID);
				if (nameComponent) {
					ImGui::Text(nameComponent->name);
				}
				else {
					ImGui::Text(("Entity " + std::to_string(m_SelectedEntity)).c_str());
				}


				EntityData& entityData = g_EntityData[m_SelectedEntity];

				for (auto& it : entityData.m_Indices) {

					uint16 compID = it.first;
					size_t index = it.second;

					BaseComponent* comp = (BaseComponent*)(&(g_Components[compID][index]));
					comp->ShowInfo();

				}
			}

			ImGui::NextColumn();
			if (ImGui::Button("Close")) result = false;
		}
		ImGui::End();
		return result;
	}

	bool ShowImGuiSystemsWindow()
	{
		bool result = true;
		if (ImGui::Begin("Systems")) {
			for (uint32 i = 0; i < g_UpdatedSystems.size(); ++i) {
				System* system = g_UpdatedSystems[i];

				ImGui::Text(("[" + std::string(system->GetName()) + "] -> " +
					std::to_string(system->m_TimeCount / system->m_UpdatesCount)).c_str());

			}
			if (ImGui::Button("Close")) result = false;
		}
		ImGui::End();
		return result;
	}

#endif

}