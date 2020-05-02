#pragma once

#include "common.h"
#include "Scene.h"

namespace jsh {

	class State {
	public:
		State() = default;
		virtual ~State() = default;

		virtual void Load() {}
		virtual void Initialize() {}

		virtual void Update(float deltaTime) {}
		virtual void FixedUpdate() {}

		virtual void Render() {}

		virtual void Close() {}
	};

}