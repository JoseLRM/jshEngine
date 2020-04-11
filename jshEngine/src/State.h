#pragma once

#include "common.h"
#include "ecs/Scene.h"

namespace jsh {

	class State {
	public:
		State() = default;
		virtual ~State() = default;

		virtual void Initialize() {}

		virtual void Update(float deltaTime) {}
		virtual void FixedUpdate() {}

		virtual void Render() {}

		virtual void Close() {}
	};

}