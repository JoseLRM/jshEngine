#pragma once

#include "..//common.h"
#include <map>
#include <vector>

namespace jsh {

	class Scene;

	typedef ID_t Entity;

	constexpr Entity INVALID_ENTITY = 0u;

	class EntityData {
		size_t handleIndex = 0u;
		Entity parent = INVALID_ENTITY;
		uint32 sonsCount = 0u;

		std::map<ID_t, size_t> m_Indices;

	public:
		EntityData() {}
		friend Scene;
		friend std::vector<EntityData>;

	};

}