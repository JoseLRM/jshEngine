#include "common.h"
#include "StateManager.h"

#include "State.h"
#include "DefaultLoadState.h"

namespace jsh {
	namespace _internal {

		void StateManager::Prepare()
		{
			if (m_NextState) {
				ClearState();

				m_State = m_NextState;
				m_NextState = nullptr;

				m_LoadingState->Load();
				m_LoadingState->Initialize();
				m_CurrentState = m_LoadingState;
				
				m_LoadingContext.Reset();
				jshTask::Execute([this]() {
					m_State->Load();
				}, &m_LoadingContext);
			}

			if (m_LoadingState && !IsLoading()) {
				m_LoadingState->Close();
				delete m_LoadingState;
				m_LoadingState = nullptr;

				m_State->Initialize();
				m_CurrentState = m_State;
			}
		}

		void StateManager::Update(float dt)
		{
			if (m_CurrentState) {
				m_CurrentState->Update(dt);
			}
		}
		void StateManager::FixedUpdate()
		{
			if (m_CurrentState) {
				m_CurrentState->FixedUpdate();
			}
		}
		void StateManager::Render()
		{
			if (m_CurrentState) {
				m_CurrentState->Render();
			}
		}

		void StateManager::LoadState(State* state, State* loadingState)
		{
			JSH_ASSERT(state != nullptr);
			if (IsLoading() || m_NextState != nullptr) {
				jshDebug::LogW("You can't load a state while is loading other");
				return;
			}

			if (loadingState == nullptr) loadingState = new DefaultLoadState();

			m_LoadingState = loadingState;
			m_NextState = state;
		}
		void StateManager::ClearState()
		{
			if (m_State) {
				m_State->Close();
				delete m_State;
				m_State = nullptr;
			}
		}

		bool StateManager::IsLoading() const
		{
			return jshTask::Running(m_LoadingContext);
		}

	}
}