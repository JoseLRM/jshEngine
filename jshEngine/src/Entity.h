#pragma once

#include "common.h"
#include <map>
#include <vector>
#include "Transform.h"

namespace jsh {

	typedef uint32 Entity;
	constexpr Entity INVALID_ENTITY = 0u;

	class EntityData {
	public:
		size_t handleIndex = 0u;
		Entity parent = INVALID_ENTITY;
		uint32 sonsCount = 0u;
		Transform transform;

		std::map<uint16, size_t> indices;

		EntityData() {}

	};

}