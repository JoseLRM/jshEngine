#pragma once

#include "common.h"
#include <functional>
#include <vector>

#define JSH_EVENT_LAYER_FOCUSED_GUI 0
#define JSH_EVENT_LAYER_GUI0		1
#define JSH_EVENT_LAYER_GUI1		2
#define JSH_EVENT_LAYER_GUI2		3
#define JSH_EVENT_LAYER_GUI3		4
#define JSH_EVENT_LAYER_GUI4		5
#define JSH_EVENT_LAYER_DEFAULT		6

namespace jsh {

	class Event {
		bool m_Dead = false;

	public:
		Event() : m_Dead(false) {}

		inline void Kill() noexcept { m_Dead = true; }
		inline bool IsDead() const noexcept { return m_Dead; }
	};

	template<typename E>
	class Listener {
	private:
		static std::vector<Listener<E>> s_List;
		std::function<bool(E&)> fn;
		mutable uint16 m_Layer;

	public:
		Listener(const std::function<bool(E&)>& fn) : m_Layer(JSH_EVENT_LAYER_DEFAULT), fn(fn) {}
		inline uint16 GetLayer() const noexcept { return m_Layer; }
		inline void SetLayer(const uint32 layer) const noexcept { m_Layer = layer; }

		inline void operator()(E& e) const { fn(e); }

		inline bool operator==(const Listener& other) const 
		{
			return fn.target<bool(E&)>() == other.fn.target<bool(E&)>() && m_Layer == other.m_Layer;
		}
		inline bool operator<(const Listener& other) const
		{
			return m_Layer > other.m_Layer;
		}
		inline bool operator>(const Listener& other) const
		{
			return m_Layer < other.m_Layer;
		}

	public:
		static void PushListener(const Listener& listener)
		{
			if (s_List.size() == 0) s_List.push_back(listener);
			else {
				auto it = std::lower_bound(s_List.begin(), s_List.end(), listener);
				s_List.insert(it, listener);
			}
		}

		static void Unregister(uint32 layer, const std::function<bool(E&)>& fn)
		{
			Listener<E> sameListener(fn);
			sameListener.SetLayer(layer);
			auto it = std::find(s_List.begin(), s_List.end(), sameListener);
			if (it != s_List.end()) {
				s_List.erase(it);
			}
		}

		static void Unregister(uint32 layer)
		{
			size_t firstIndex = 0;
			size_t size = 0;
			for (int32 i = 0; i < s_List.size(); ++i) {
				if (s_List[i].m_Layer == layer) {
					if(size == 0) firstIndex = i;
					size++;
				}
				else if (s_List[i].m_Layer < layer) {
					break;
				}
			}

			if (size == 0) return;

			for (int32 i = firstIndex + size - 1; i >= firstIndex; --i) {
				s_List.erase(s_List.begin() + i);
			}
		}

		static void Call(E& e)
		{
			for (int32 i = s_List.size() - 1; i >= 0; --i) {
				if (!s_List[i].fn(e)) {
					s_List.erase(s_List.begin() + i);
				}
				if (e.IsDead()) break;
			}
		}

	};

	template<typename E>
	std::vector<Listener<E>> Listener<E>::s_List;

}

namespace jshEvent {

	template<typename L>
	void Register(uint32 layer, const L& listener)
	{
		listener.SetLayer(layer);
		L::PushListener(listener);
	}

	template<typename E>
	void Unregister(uint32 layer, const std::function<bool(E&)>& fn)
	{
		jsh::Listener<E>::Unregister(layer, fn);
	}

	template<typename L>
	void Unregister(uint32 layer)
	{
		jsh::Listener<L>::Unregister(layer);
	}

	template<typename E>
	void Dispatch(E& e)
	{
		jsh::Listener<E>::Call(e);
	}

}

#define jshEventDefineListener(listener, e) struct listener : public jsh::Listener<e> { listener(const std::function<bool(e&)> fn) : jsh::Listener<e>(fn){} }
