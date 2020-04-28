#pragma once

#include "common.h"
#include <map>
#include <vector>
#include "Transform.h"

namespace jsh {

	typedef uint32 Entity;
	constexpr Entity INVALID_ENTITY = 0u;

	struct Layer {
		int16 value = 0;
		const char* name = nullptr;
		uint16 ID = 0;
		inline operator int32() { return value; }
		inline bool SameLayer(const Layer& other) const noexcept { return other.ID == ID; }
		inline bool operator==(const Layer& other) const noexcept { return value == other.value; }
		inline bool operator!=(const Layer& other) const noexcept { return value != other.value; }
		inline bool operator<(const Layer& other) const noexcept { return value < other.value; }
		inline bool operator>(const Layer& other) const noexcept { return value > other.value; }
		inline bool operator<=(const Layer& other) const noexcept { return value <= other.value; }
		inline bool operator>=(const Layer& other) const noexcept { return value >= other.value; }
	};

	class EntityData {
	public:
		size_t handleIndex = 0u;
		Entity parent = INVALID_ENTITY;
		uint32 sonsCount = 0u;
		Transform transform;
		Layer* layer;

		std::map<uint16, size_t> indices;
	};

}