#include "Scene.h"
#include "..//TaskSystem.h"

namespace jsh {

	uint32 System::s_SystemCount = 0u;
	jsh::vector<size_t> Scene::s_ComponentsSize;

	Scene::Scene() {}

	void Scene::Create() noexcept
	{
		for (ID_t id = 0; id < BaseComponent::GetComponentsCount(); ++id) {
			m_Components.push_back(jsh::vector<byte>());
		}
		CreateEntity(); // allocate invalid Entity :)

		if (s_ComponentsSize.empty()) {
			s_ComponentsSize.resize(BaseComponent::GetComponentsCount());
		}
	}

	Scene::~Scene()
	{
		for (ID_t id = 0; id < BaseComponent::GetComponentsCount(); ++id) {
			m_Components[id].clear();
		}
		m_Components.clear();
		m_Entities.clear();
		m_EntityData.clear();
	}

#pragma region ENTITIES
	// -------------------------ENTITIES--------------------------

	Entity Scene::CreateEntity() noexcept
	{
		Entity entity;
		if (m_FreeEntityData.size() != 0) {
			entity = m_FreeEntityData.back();
			m_FreeEntityData.pop_back();

			EntityData& entityData = m_EntityData[entity];
			entityData.handleIndex = m_Entities.size();
		}
		else {
			entity = m_EntityData.size();

			EntityData entityData;
			entityData.handleIndex = m_Entities.size();

			if (m_EntityData.capacity() == m_EntityData.size()) m_EntityData.reserve(10u);
			m_EntityData.push_back(entityData);
		}
		
		m_Entities.push_back(entity, 10u);
		return entity;
	}
	void Scene::CreateEntities(uint32 cant, jsh::vector<Entity>* entities) noexcept
	{
		if (entities) entities->reserve(cant);

		size_t entityIndex = m_Entities.size();
		m_Entities.resize(m_Entities.size() + cant);
		while (!m_FreeEntityData.empty() && cant > 0) {
			Entity entity = m_FreeEntityData.back();
			m_FreeEntityData.pop_back();
			cant--;

			EntityData& entityData = m_EntityData[entity];
			entityData.handleIndex = entityIndex++;

			m_Entities.push_back_nr(entity);
			if (entities) entities->push_back_nr(entity);
		}

		m_EntityData.reserve(cant);
		Entity beginEntity = m_EntityData.size();
		for (uint32 i = 0; i < cant; ++i) {
			EntityData ed;
			ed.handleIndex = entityIndex + i;
			m_EntityData.push_back(ed);
			Entity entity = beginEntity + i;
			m_Entities.push_back(entity);
			if (entities) entities->push_back_nr(entity);
		}
	}

	Entity Scene::CreateSEntity(Entity parent) noexcept
	{
		EntityData& parentData = m_EntityData[parent];

		// update parents count
		jsh::vector<Entity> parentsToUpdate;
		parentsToUpdate.push_back(parent, 3);
		if (!parentsToUpdate.empty()) {

			Entity e = parentsToUpdate.back();
			parentsToUpdate.pop_back();
			EntityData& eData = m_EntityData[e];
			eData.sonsCount++;

			if (eData.parent != INVALID_ENTITY) parentsToUpdate.push_back(eData.parent, 3);
		}

		Entity entity;
		if (m_FreeEntityData.size() != 0) {
			entity = m_FreeEntityData.back();
			m_FreeEntityData.pop_back();
		}
		else {
			entity = m_EntityData.size();

			if (m_EntityData.capacity() == m_EntityData.size()) m_EntityData.reserve(10u);
			m_EntityData.push_back(EntityData());
		}
		
		EntityData& entityData = m_EntityData[entity];
		entityData.parent = parent;
		size_t index = parentData.handleIndex + parentData.sonsCount;
		entityData.handleIndex = index;

		// special case, the parent and sons are in back of the list
		if (index == m_Entities.size()) {
			m_Entities.push_back(entity, 10u);
			return entity;
		}

		if (m_Entities.reserve_request()) {
			m_Entities.reserve(10u);
		}

		m_Entities.insert(index, entity);

		for (size_t i = index + 1; i < m_EntityData.size(); ++i) {
			m_EntityData[i].handleIndex++;
		}

		return entity;
	}
	
	void Scene::CreateSEntities(Entity parent, uint32 cant, jsh::vector<Entity>* entities) noexcept
	{
		// update parents count
		jsh::vector<Entity> parentsToUpdate;
		parentsToUpdate.push_back(parent, 3);
		while (!parentsToUpdate.empty() && cant > 0) {

			Entity e = parentsToUpdate.back();
			parentsToUpdate.pop_back();
			EntityData& eData = m_EntityData[e];
			eData.sonsCount += cant;

			if (eData.parent != INVALID_ENTITY) parentsToUpdate.push_back(eData.parent, 3);
		}

		// reserve memory
		m_Entities.reserve(cant);

		EntityData& parentData = m_EntityData[parent];
		size_t entityIndex = parent + parentData.sonsCount - cant + 1;

		// if the parent and sons aren't in back of the list
		if (entityIndex != m_Entities.size()) {
			size_t newDataIndex = parent + parentData.sonsCount + 1;
			// move old entities
			for (size_t i = m_Entities.capacity() - 1; i >= newDataIndex; --i) {
				m_Entities[i] = m_Entities[i - cant];
				m_EntityData[m_Entities[i]].handleIndex = i;
			}
		}

		// creating entities
		if (entities) entities->reserve(cant);
		while (!m_FreeEntityData.empty()) {
			Entity entity = m_FreeEntityData.back();
			m_FreeEntityData.pop_back();
			cant--;

			EntityData& entityData = m_EntityData[entity];
			entityData.handleIndex = entityIndex++;
			entityData.parent = parent;
			m_Entities[entityData.handleIndex] = entity;
			if (entities) entities->push_back_nr(entity);
		}

		Entity firstEntity = m_EntityData.size();
		m_EntityData.reserve(cant);
		for (uint32 i = 0; i < cant; ++i) {

			Entity entity = firstEntity + i;
			EntityData ed;
			ed.handleIndex = entityIndex + i;
			ed.parent = parent;
			m_EntityData.push_back(ed);
			m_Entities[ed.handleIndex] = entity;
			if (entities) entities->push_back_nr(entity);
		}
		m_Entities.add_pos(cant);
	}

	void Scene::DestroyEntity(Entity entity) noexcept
	{
		EntityData& entityData = m_EntityData[entity];
		uint32 cant = entityData.sonsCount + 1;
		
		// notify parents
		if (entityData.parent != INVALID_ENTITY) {
			jsh::vector<Entity> updateInheritance;
			updateInheritance.push_back(entityData.parent);
			while (!updateInheritance.empty()) {

				Entity p = updateInheritance.back();
				updateInheritance.pop_back();

				EntityData& pData = m_EntityData[entity];
				pData.sonsCount -= cant;

				if (pData.parent != INVALID_ENTITY) updateInheritance.push_back(pData.parent, 3);
				
			}
		}

		// data to remove entities
		size_t indexBeginDest = entityData.handleIndex;
		size_t indexBeginSrc = entityData.handleIndex + cant;
		size_t cpyCant = m_Entities.size() - indexBeginSrc;

		// remove components & entityData
		m_FreeEntityData.reserve(cant);
		for (uint32 i = 0; i < cant; i++) {
			Entity e = m_Entities[indexBeginDest + i];
			EntityData& ed = m_EntityData[e];
			RemoveComponents(ed);
			ed.handleIndex = 0;
			ed.parent = INVALID_ENTITY;
			m_FreeEntityData.push_back_nr(e);
		}

		// remove from entities & update indices
		memcpy(&m_Entities[indexBeginDest], &m_Entities[indexBeginSrc], cpyCant * sizeof(Entity));
		m_Entities.sub_pos(cant);
		for (size_t i = indexBeginDest; i < m_Entities.size(); ++i) {
			m_EntityData[m_Entities[i]].handleIndex = i;
		}
		
	}
#pragma endregion

#pragma region COMPONENTS
	//--------------------------COMPONENTS--------------------------
	void Scene::AddComponent(Entity entity, BaseComponent* comp, ID_t componentID, size_t componentSize) noexcept
	{
		comp->entityID = entity;
		auto& list = m_Components[componentID];
		size_t index = list.size();

		// register the size
		if (list.empty()) s_ComponentsSize[componentID] = componentSize;

		// allocate the component
		if (list.capacity() < index + componentSize) {
			list.reserve(componentSize * 5);
		}
		list.add_pos(componentSize);
		memcpy(&list[index], comp, componentSize);

		// set index in entity
		m_EntityData[entity].m_Indices[componentID] = index;
	}

	void Scene::AddComponents(jsh::vector<Entity>& entities, BaseComponent* comp, ID_t componentID, size_t componentSize) noexcept
	{
		auto& list = m_Components[componentID];
		size_t index = list.size();

		// register the size
		if (list.empty()) s_ComponentsSize[componentID] = componentSize;

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
			m_EntityData[currentEntity].m_Indices[componentID] = currentIndex;
		}
	}

	void Scene::RemoveComponent(Entity entity, ID_t componentID, size_t componentSize) noexcept
	{
		EntityData& entityData = m_EntityData[entity];
		auto it = entityData.m_Indices.find(componentID);
		if (it == entityData.m_Indices.end()) return;

		size_t index = (*it).second;
		entityData.m_Indices.erase(it);

		auto& list = m_Components[componentID];

		// if the component isn't the last element
		if (index != list.size() - componentSize) {
			// set back data in index
			memcpy(&list[index], &list[list.size() - componentSize], componentSize);

			Entity otherEntity = ((BaseComponent*)(&list[index]))->entityID;
			m_EntityData[otherEntity].m_Indices[componentID] = index;
		}

		list.sub_pos(componentSize);
	}

	void Scene::RemoveComponents(EntityData& entityData) noexcept
	{
		ID_t componentID;
		size_t componentSize, index;
		for (auto& it : entityData.m_Indices) {
			componentID = it.first;
			componentSize = s_ComponentsSize[componentID];
			index = it.second;
			auto& list = m_Components[componentID];

			if (index != list.size() - componentSize) {
				// set back data in index
				memcpy(&list[index], &list[list.size() - componentSize], componentSize);

				Entity otherEntity = ((BaseComponent*)(&list[index]))->entityID;
				m_EntityData[otherEntity].m_Indices[componentID] = index;
			}

			list.sub_pos(componentSize);
		}
		entityData.m_Indices.clear();
	}
#pragma endregion

#pragma region SYSTEMS
	//------------------SYSTEMS------------------

	void Scene::UpdateSystem(System* system, float dt)
	{
		if(system->GetExecuteType() == JSH_ECS_SYSTEM_SAFE 
			|| system->GetExecuteType() == JSH_ECS_SYSTEM_PARALLEL) {

			if (system->IsIndividualSystem()) UpdateIndividualSafeSystem(system, dt);
			else UpdateCollectiveAndMultithreadedSystem(system, dt);
			
		}
		else {
			if (system->IsIndividualSystem()) UpdateCollectiveAndMultithreadedSystem(system, dt);
		}
	}

	void Scene::UpdateSystems(System** systems, uint32 cant, float dt)
	{
		System* pSystem;

		// update safe systems
		for (uint32 i = 0; i < cant; ++i) {
			pSystem = systems[i];
			if (pSystem->GetExecuteType() == JSH_ECS_SYSTEM_SAFE) {
				if (pSystem->IsIndividualSystem()) UpdateIndividualSafeSystem(pSystem, dt);
				else UpdateCollectiveAndMultithreadedSystem(pSystem, dt);
			}
		}

		// update multithreaded systems
		for (uint32 i = 0; i < cant; ++i) {
			pSystem = systems[i];
			if (pSystem->GetExecuteType() == JSH_ECS_SYSTEM_MULTITHREADED) {
				if (pSystem->IsIndividualSystem()) UpdateCollectiveAndMultithreadedSystem(pSystem, dt);
			}
		}

		// update parallel systems
		for (uint32 i = 0; i < cant; ++i) {
			pSystem = systems[i];
			if (pSystem->GetExecuteType() == JSH_ECS_SYSTEM_PARALLEL) {
				if (pSystem->IsIndividualSystem()) 
					jshTask::Execute([pSystem, dt, this]() { UpdateIndividualSafeSystem(pSystem, dt); });
				else jshTask::Execute([pSystem, dt, this]() { UpdateCollectiveAndMultithreadedSystem(pSystem, dt); });
			}
		}

		jshTask::Wait();
	}

	// internal
	void Scene::UpdateIndividualSafeSystem(System* system, float dt)
	{
		// system requisites
		auto& request = system->GetRequestedComponents();
		auto& optional = system->GetOptionalComponents();
		uint32 cantOfComponents = request.size() + optional.size();

		if (request.size() == 0) {
			jshLogW("System '%s' haven't requested components", system->GetName());
			return;
		}

		// for optimization, choose the sortest component list
		uint32 indexOfBestList = 0;
		ID_t idOfBestList = request[0];
		for (uint32 i = 1; i < request.size(); ++i) {
			if (m_Components[request[i]].size() < m_Components[idOfBestList].size()) {
				idOfBestList = request[i];
				indexOfBestList = i;
			}
		}
		// if one request is empty, exit
		auto list = m_Components[idOfBestList];
		if (list.size() == 0) return;
		size_t sizeOfBestList = s_ComponentsSize[idOfBestList];

		// reserve memory for the pointers
		jsh::vector<BaseComponent*> components;
		components.reserve(cantOfComponents);
		components.add_pos(cantOfComponents);

		// for all the entities
		BaseComponent* compOfBestList;
		if (system->InTreeMode()) { // entities sorted by inheritance
			for (size_t i = 0; i < m_Entities.size(); ++i) {
				Entity entity = m_Entities[i];
				compOfBestList = GetComponent(m_EntityData[entity], idOfBestList);
				
				bool isValid = compOfBestList;
				if (isValid) {
					EntityData& entityData = m_EntityData[entity];
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
						i += m_EntityData[entity].sonsCount;
						continue;
					}
					// allocate optional components
					for (j = 0; j < optional.size(); ++j) {

						BaseComponent* comp = GetComponent(entityData, optional[j]);
						components[j + request.size()] = comp;
					}

					// if the entity is valid, call update
					system->UpdateEntity(*this, entity, components.data(), dt);
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
				EntityData& entityData = m_EntityData[entity];
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
				system->UpdateEntity(*this, entity, components.data(), dt);
			}
		}		
	}

	void Scene::UpdateCollectiveAndMultithreadedSystem(System* system, float dt)
	{
		jsh::vector<BaseComponent**> componentsList;
		// system requisites
		auto& request = system->GetRequestedComponents();
		auto& optional = system->GetOptionalComponents();
		uint32 cantOfComponents = request.size() + optional.size();

		if (request.size() == 0) {
			jshLogW("System '%s' haven't requested components", system->GetName());
			return;
		}

		// for optimization, choose the sortest component list
		uint32 indexOfBestList = 0;
		ID_t idOfBestList = request[0];
		for (uint32 i = 1; i < request.size(); ++i) {
			if (m_Components[request[i]].size() < m_Components[idOfBestList].size()) {
				idOfBestList = request[i];
				indexOfBestList = i;
			}
		}
		// if one request is empty, exit
		auto list = m_Components[idOfBestList];
		if (list.size() == 0) return;
		size_t sizeOfBestList = s_ComponentsSize[idOfBestList];

		// for all the entities
		BaseComponent* compOfBestList;
		BaseComponent** components = new BaseComponent * [cantOfComponents];

		if (system->InTreeMode() && system->IsCollectiveSystem()) {
			for (size_t i = 0; i < m_Entities.size(); ++i) {

				Entity entity = m_Entities[i];
				compOfBestList = GetComponent(m_EntityData[entity], idOfBestList);

				bool isValid = compOfBestList;
				if (isValid) {
					EntityData& entityData = m_EntityData[entity];
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
				EntityData& entityData = m_EntityData[compOfBestList->entityID];

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
			jshTask::Async(componentsList.size(), jshTask::ThreadCount(), [this, &componentsList, system, dt](ThreadArgs args) {

				float deltaTime = dt;
				BaseComponent** components = componentsList[args.index];
				Entity entity = components[0]->entityID;
				system->UpdateEntity(*this, entity, components, deltaTime);

			});

			jshTask::Wait();
		}
		else {
			system->UpdateEntities(*this, componentsList, dt);
		}

		// free memory
		for (size_t i = 0; i < componentsList.size(); ++i) {
			delete componentsList[i];
		}
	}

#pragma endregion
	
}