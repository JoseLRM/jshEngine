#include "Scene.h"
#include "TaskSystem.h"
#include "Timer.h"

namespace jsh {
	uint32 System::s_SystemCount = 0;
}

using namespace jsh;

namespace jshScene {

	using namespace _internal;

	jsh::vector<Entity> g_Entities;
	std::vector<EntityData> g_EntityData;
	jsh::vector<Entity> g_FreeEntityData;

	std::vector<std::vector<byte>> g_Components;

	PerformanceTest g_PerformanceTest;
	
	std::map<const char*, std::unique_ptr<Layer>> g_Layers;

	namespace _internal {
		jsh::vector<jsh::Entity>& GetEntitiesList()
		{
			return g_Entities;
		}
		std::vector<jsh::EntityData>& GetEntityDataList()
		{
			return g_EntityData;
		}
		std::vector<std::vector<byte>>& GetComponentsList()
		{
			return g_Components;
		}

		BaseComponent* GetComponent(jsh::Entity e, uint16 componentID) noexcept
		{
			jsh::EntityData& entity = g_EntityData[e];
			auto it = entity.indices.find(componentID);
			if (it != entity.indices.end()) return (BaseComponent*)(&(g_Components[componentID][it->second]));

			return nullptr;
		}
		BaseComponent* GetComponent(const jsh::EntityData& entity, uint16 componentID) noexcept
		{
			auto it = entity.indices.find(componentID);
			if (it != entity.indices.end()) return (BaseComponent*)(&(g_Components[componentID][it->second]));

			return nullptr;
		}
	}

	void Initialize() noexcept
	{
		g_Components.reserve(GetComponentsCount());
		for (uint16 id = 0; id < GetComponentsCount(); ++id) {
			g_Components.push_back(std::vector<byte>());
		}
		// Create Layers
		jshScene::CreateLayer("Default", 0);

		CreateEntity(); // allocate invalid Entity :)
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

			g_EntityData[entity].handleIndex = g_Entities.size();
		}
		else {
			entity = Entity(g_EntityData.size());

			if (g_EntityData.capacity() == g_EntityData.size()) g_EntityData.reserve(10u);
			g_EntityData.push_back(EntityData());
			g_EntityData.back().handleIndex = g_Entities.size();
		}

		g_EntityData[entity].transform.entity = entity;
		g_EntityData[entity].layer = JSH_DEFAULT_LAYER;
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
			entityData.transform.entity = entity;

			g_Entities.push_back_nr(entity);
			if (entities) entities->push_back_nr(entity);
		}

		g_EntityData.reserve(cant);
		Entity beginEntity = Entity(g_EntityData.size());
		for (uint32 i = 0; i < cant; ++i) {
			Entity entity = beginEntity + i;
			g_Entities.push_back(entity);
			EntityData ed;
			ed.handleIndex = entityIndex + i;
			ed.layer = JSH_DEFAULT_LAYER;
			g_EntityData.push_back(ed);
			ed.transform.entity = entity;
			if (entities) entities->push_back_nr(entity);
		}
	}

	Entity CreateSEntity(Entity parent) noexcept
	{
		Entity entity;

		// update parents count
		jsh::vector<Entity> parentsToUpdate;
		parentsToUpdate.push_back(parent, 3);
		while (!parentsToUpdate.empty()) {

			Entity parentToUpdate = parentsToUpdate.back();
			parentsToUpdate.pop_back();
			EntityData& parentToUpdateEd = g_EntityData[parentToUpdate];
			parentToUpdateEd.sonsCount++;

			if (parentToUpdateEd.parent != INVALID_ENTITY) parentsToUpdate.push_back(parentToUpdateEd.parent, 3);
		}

		if (g_FreeEntityData.size() != 0) {
			entity = g_FreeEntityData.back();
			g_FreeEntityData.pop_back();
		}
		else {
			entity = Entity(g_EntityData.size());

			if (g_EntityData.capacity() == g_EntityData.size()) g_EntityData.reserve(10u);
			g_EntityData.push_back(EntityData());
		}

		EntityData& parentData = g_EntityData[parent];
		EntityData& entityData = g_EntityData[entity];
		entityData.parent = parent;
		size_t index = parentData.handleIndex + parentData.sonsCount;
		entityData.handleIndex = index;
		entityData.transform.entity = entity;
		entityData.layer = JSH_DEFAULT_LAYER;

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
			entityData.transform.entity = entity;
			entityData.layer = JSH_DEFAULT_LAYER;
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
			ed.transform.entity = entity;
			ed.layer = JSH_DEFAULT_LAYER;
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

				EntityData& pData = g_EntityData[p];
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
			ed.sonsCount = 0u;
			ed.layer = JSH_DEFAULT_LAYER;
			g_FreeEntityData.push_back_nr(e);
		}

		// remove from entities & update indices
		memcpy(&g_Entities[indexBeginDest], &g_Entities[indexBeginSrc], cpyCant * sizeof(Entity));
		g_Entities.sub_pos(cant);
		for (size_t i = indexBeginDest; i < g_Entities.size(); ++i) {
			g_EntityData[g_Entities[i]].handleIndex = i;
		}

	}

	namespace _internal {
		jsh::Entity DuplicateEntity(jsh::Entity duplicated, jsh::Entity parent)
		{
			Entity copy;

			if (parent == INVALID_ENTITY) copy = CreateEntity();
			else copy = CreateSEntity(parent);

			EntityData& duplicatedEd = g_EntityData[duplicated];
			EntityData& copyEd = g_EntityData[copy];

			for (auto& it : duplicatedEd.indices) {
				uint16 ID = it.first;
				size_t SIZE = GetComponentSize(ID);

				auto& list = g_Components[ID];

				size_t index = list.size();
				list.resize(index + SIZE);

				BaseComponent* comp = GetComponent(duplicated, ID);
				memcpy(&list[index], comp, SIZE);

				((BaseComponent*)(&list[index]))->entity = copy;
				copyEd.indices[ID] = index;
			}

			copyEd.transform = duplicatedEd.transform;
			copyEd.transform.entity = copy;

			for (uint32 i = 0; i < g_EntityData[duplicated].sonsCount; ++i) {
				Entity toCopy = g_Entities[g_EntityData[duplicated].handleIndex + i + 1];
				DuplicateEntity(toCopy, copy);
				i += g_EntityData[toCopy].sonsCount;
			}

			return copy;
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

	jsh::Transform& GetTransform(jsh::Entity entity)
	{
		return g_EntityData[entity].transform;
	}

	////////////////////////////////COMPONENTS////////////////////////////////
	namespace _internal {
		void AddComponent(Entity entity, BaseComponent* comp, uint16 componentID, size_t componentSize) noexcept
		{
			auto& list = g_Components[componentID];
			size_t index = list.size();

			// allocate the component
			list.resize(list.size() + componentSize);
			memcpy(&list[index], comp, componentSize);
			((BaseComponent*)& list[index])->entity = entity;

			// set index in entity
			g_EntityData[entity].indices[componentID] = index;
		}

		void AddComponents(jsh::vector<Entity>& entities, BaseComponent* comp, uint16 componentID, size_t componentSize) noexcept
		{
			auto& list = g_Components[componentID];
			size_t index = list.size();

			// allocate the components
			list.resize(list.size() + componentSize * entities.size());

			size_t currentIndex;
			Entity currentEntity;
			for (uint32 i = 0; i < entities.size(); ++i) {
				currentIndex = index + (i * componentSize);
				currentEntity = entities[i];

				memcpy(&list[currentIndex], comp, componentSize);
				// set entity in component
				BaseComponent* component = (BaseComponent*)(&list[currentIndex]);
				component->entity = currentEntity;
				// set index in entity
				g_EntityData[currentEntity].indices[componentID] = currentIndex;
			}
		}

		void RemoveComponent(Entity entity, uint16 componentID, size_t componentSize) noexcept
		{
			EntityData& entityData = g_EntityData[entity];
			auto it = entityData.indices.find(componentID);
			if (it == entityData.indices.end()) return;

			size_t index = (*it).second;
			entityData.indices.erase(it);

			auto& list = g_Components[componentID];

			// if the component isn't the last element
			if (index != list.size() - componentSize) {
				// set back data in index
				memcpy(&list[index], &list[list.size() - componentSize], componentSize);

				Entity otherEntity = ((BaseComponent*)(&list[index]))->entity;
				g_EntityData[otherEntity].indices[componentID] = index;
			}

			list.resize(list.size() - componentSize);
		}

		void RemoveComponents(EntityData& entityData) noexcept
		{
			uint16 componentID;
			size_t componentSize, index;
			for (auto& it : entityData.indices) {
				componentID = it.first;
				componentSize = jshScene::GetComponentSize(componentID);
				index = it.second;
				auto& list = g_Components[componentID];

				if (index != list.size() - componentSize) {
					// set back data in index
					memcpy(&list[index], &list[list.size() - componentSize], componentSize);

					Entity otherEntity = ((BaseComponent*)(&list[index]))->entity;
					g_EntityData[otherEntity].indices[componentID] = index;
				}

				list.resize(list.size() - componentSize);
			}
			entityData.indices.clear();
		}
	}

	////////////////////////////////SYSTEMS////////////////////////////////

	void UpdateSystem(System* system, float dt)
	{
		if (system->GetExecuteType() == JSH_ECS_SYSTEM_SAFE
			|| system->GetExecuteType() == JSH_ECS_SYSTEM_PARALLEL) {

			if (system->IsIndividualSystem()) UpdateIndividualSafeSystem(system, dt);
			else UpdateCollectiveAndMultithreadedSystem(system, dt);

		}
		else {
			if (system->IsIndividualSystem()) UpdateCollectiveAndMultithreadedSystem(system, dt);
		}
	}

	void UpdateSystems(System** systems, uint32 cant, float dt)
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
					jshTask::Execute([pSystem, dt]() {

					UpdateIndividualSafeSystem(pSystem, dt);

				});
				else jshTask::Execute([pSystem, dt]() {

					UpdateCollectiveAndMultithreadedSystem(pSystem, dt);

				});
			}
		}

		jshTask::Wait();
	}

	jsh::Time GetSystemPerformance(const jsh::System& system)
	{
		return g_PerformanceTest.Get(system.GetSystemID());
	}

	namespace _internal {
		void UpdateIndividualSafeSystem(System* system, float dt)
		{
			g_PerformanceTest.Begin(system->GetSystemID());

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
			size_t sizeOfBestList = jshScene::GetComponentSize(idOfBestList);

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
					Entity entity = compOfBestList->entity;
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
			g_PerformanceTest.End(system->GetSystemID());
		}

		void UpdateCollectiveAndMultithreadedSystem(System* system, float dt)
		{
			g_PerformanceTest.Begin(system->GetSystemID());

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
			size_t sizeOfBestList = jshScene::GetComponentSize(idOfBestList);

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
					EntityData& entityData = g_EntityData[compOfBestList->entity];

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

					system->UpdateEntity(componentsList[args.index][0]->entity, componentsList[args.index], dt);

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

			g_PerformanceTest.End(system->GetSystemID());
		}
	}

	//////////////////////////////////LAYERS///////////////////////////////////////
	void CreateLayer(const char* name, int16 value)
	{
		Layer layer;
		layer.value = value;
		layer.name = name;

		auto it = g_Layers.find(name);
		if (it != g_Layers.end()) {
			jshLogW("Repeated Layer '%s'", name);
			return;
		}

		g_Layers[name] = std::make_unique<Layer>(layer);

		// SET IDS
		// sort
		jsh::vector<Layer*> layers;
		layers.reserve(g_Layers.size());
		for (auto& it : g_Layers) {
			layers.push_back_nr(it.second.get());
		}
		std::sort(layers.data(), layers.data() + layers.size() - 1u, [](Layer* layer0, Layer* layer1) {
			return (*layer0) < (*layer1);
		});

		// set
		for (uint16 id = 0; id < layers.size(); ++id) {
			layers[id]->ID = id;
		}
	}
	jsh::Layer* GetLayer(const char* name)
	{
		auto it = g_Layers.find(name);
		if (it == g_Layers.end()) {
			jshLogE("Layer not found '%s'", name);
			return nullptr;
		}
		return (*it).second.get();
	}
	void DestroyLayer(const char* name)
	{
		auto it = g_Layers.find(name);
		if (it == g_Layers.end()) {
			jshLogE("Layer not found '%s'", name);
		}
		g_Layers.erase(it);
	}
	uint32 GetLayerCount()
	{
		return g_Layers.size();
	}

}
	//////////////////////////////////DEBUG////////////////////////////////

#if defined(JSH_IMGUI)

namespace jshScene {

	jsh::Entity g_SelectedEntity = INVALID_ENTITY;
	bool g_RemoveOutline = false;

	namespace _internal {
		void ImGuiAddEntity(jsh::Entity entity)
		{
			EntityData& entityData = g_EntityData[entity];

			bool empty = entityData.sonsCount == 0;
			auto treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | (empty ? ImGuiTreeNodeFlags_Bullet : ImGuiTreeNodeFlags_AllowItemOverlap);

			bool active;
			NameComponent* nameComponent = (NameComponent*)GetComponent(entity, NameComponent::ID);
			if (nameComponent) {
				active = ImGui::TreeNodeEx((std::string(nameComponent->name) + "[" + std::to_string(entity) + "]").c_str(), treeFlags);
			}
			else {
				active = ImGui::TreeNodeEx(("Entity[" + std::to_string(entity) + "]").c_str(), treeFlags);
			}

			if (ImGui::IsItemClicked()) {
				if (g_SelectedEntity != entity) {
					if (g_SelectedEntity != INVALID_ENTITY) {
						if (jshScene::GetComponent<OutlineComponent>(g_SelectedEntity) != nullptr) {
							if(g_RemoveOutline) jshScene::RemoveComponent<OutlineComponent>(g_SelectedEntity);
						}
					}
					if (jshScene::GetComponent<OutlineComponent>(entity) == nullptr) {
						jshScene::AddComponent(entity, OutlineComponent());
						g_RemoveOutline = true;
					}
					else g_RemoveOutline = false;
					g_SelectedEntity = entity;
				}
			}
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
		static bool showEntityData = false;

		if (showEntityData) {
			if (ImGui::Begin("EntityData")) {
				for (uint32 i = 1; i < g_EntityData.size(); ++i) {
					EntityData& ed = g_EntityData[i];
					ImGui::Text("Entity index = %u", ed.handleIndex);
					ImGui::Text("Sons count = %u", ed.sonsCount);
					ImGui::Text("Parent = %u", ed.parent);
					ImGui::Text("Position = { %f, %f, %f }", ed.transform.GetWorldPosition().x, ed.transform.GetWorldPosition().y, ed.transform.GetWorldPosition().z);
					ImGui::Text("Layer = %s(%i)", ed.layer->name, ed.layer->value);

					ImGui::Separator();
				}
			}
			ImGui::End();
		}

		if (ImGui::Begin("Entities")) {
			
			ImGui::BeginChild("Entities");
			ImGui::Columns(2);
			{
				ImGui::BeginChild("Entities");

				for (size_t i = 1; i < g_Entities.size(); ++i) {

					Entity entity = g_Entities[i];
					EntityData& entityData = g_EntityData[entity];

					if (entityData.parent == INVALID_ENTITY) {
						ImGuiAddEntity(entity);
						i += entityData.sonsCount;
					}

				}

				if (g_SelectedEntity < 0 || g_SelectedEntity >= g_EntityData.size()) g_SelectedEntity = INVALID_ENTITY;

				ImGui::EndChild();
			}

			ImGui::NextColumn();

			{
				ImGui::BeginChild("Components");

				if (g_SelectedEntity != INVALID_ENTITY) {

					NameComponent* nameComponent = (NameComponent*)GetComponent(g_SelectedEntity, NameComponent::ID);
					if (nameComponent) {
						ImGui::Text("%s[%u]", nameComponent->name, g_SelectedEntity);
					}
					else {
						ImGui::Text("Entity[%u]", g_SelectedEntity);
					}

					if (ImGui::Button("Destroy")) {
						DestroyEntity(g_SelectedEntity);
						g_SelectedEntity = INVALID_ENTITY;
						g_RemoveOutline = false;
					}
					else {

						if (ImGui::BeginCombo("Add", "Add Component")) {

							for (uint16 ID = 0; ID < jshScene::GetComponentsCount(); ++ID) {
								const char* NAME = jshScene::GetComponentName(ID);
								if (GetComponent(g_SelectedEntity, ID) != nullptr) continue;
								size_t SIZE = jshScene::GetComponentSize(ID);
								if (ImGui::Button(NAME)) {
									BaseComponent* bytes = reinterpret_cast<BaseComponent*>(malloc(SIZE));
									jshScene::ConstructComponent(ID, bytes);
									jshScene::_internal::AddComponent(g_SelectedEntity, bytes, ID, SIZE);
								}
							}

							ImGui::EndCombo();
						}
						if (ImGui::BeginCombo("Rmv", "Remove Component")) {

							EntityData& ed = g_EntityData[g_SelectedEntity];
							for (auto& it : ed.indices) {
								uint16 ID = it.first;
								const char* NAME = jshScene::GetComponentName(ID);
								size_t SIZE = jshScene::GetComponentSize(ID);
								if (ImGui::Button(NAME)) {
									RemoveComponent(g_SelectedEntity, ID, SIZE);
									break;
								}
							}

							ImGui::EndCombo();
						}
						if (ImGui::Button("Duplicate")) {
							jshScene::DuplicateEntity(g_SelectedEntity);
						}
						if (ImGui::Button("Create Child")) {
							jshScene::CreateSEntity(g_SelectedEntity);
						}

						EntityData& entityData = g_EntityData[g_SelectedEntity];

						entityData.transform.ShowInfo();

						ImGui::Separator();
						ImGui::Separator();

						ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

						for (auto& it : entityData.indices) {

							uint16 compID = it.first;
							size_t index = it.second;

							BaseComponent* comp = (BaseComponent*)(&(g_Components[compID][index]));

							ImGui::Separator();
							bool remove = false;
							if (ImGui::TreeNodeEx(GetComponentName(compID), flags)) {
								comp->ShowInfo();
								ImGui::TreePop();
							}

							if (remove) {
								RemoveComponent(g_SelectedEntity, compID, GetComponentSize(compID));
								break;
							}
							ImGui::Separator();
						}
					}
				}

				ImGui::EndChild();
			}

			ImGui::NextColumn();

			ImGui::EndChild();
			if (ImGui::Button("Create")) {
				jshScene::CreateEntity();
			}
			ImGui::SameLine();
			if (ImGui::Button("Show EntityData")) {
				showEntityData = !showEntityData;
			}
			
		}
		ImGui::End();
		return result;
	}

	bool ShowImGuiSystemsWindow()
	{
		bool result = true;
		if (ImGui::Begin("Systems")) {
			
			

		}
		ImGui::End();
		return result;
	}

}
#endif
