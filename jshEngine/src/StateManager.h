#pragma once

#include "TaskSystem.h"

namespace jsh {

	class State;

	namespace _internal {

		class StateManager {
			State* m_State			= nullptr;
			State* m_LoadingState	= nullptr;
			State* m_CurrentState	= nullptr;
			State* m_NextState		= nullptr;
			
			ThreadContext m_LoadingContext;

		public:
			void Prepare();
			void Update(float deltaTime);
			void FixedUpdate();
			void Render();

			void LoadState(State* state, State* loadingState);
			void ClearState();

			inline State* GetCurrentState() noexcept { return m_CurrentState; }

		private:
			bool IsLoading() const;

		};

	}
}