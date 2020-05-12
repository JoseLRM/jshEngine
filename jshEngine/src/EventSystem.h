#pragma once

#include "common.h"
#include <functional>
#include <vector>

#include "GraphicsAdapter.h"

#define JSH_EVENT_LAYER_SYSTEM		0
#define JSH_EVENT_LAYER_GUI			1
#define JSH_EVENT_LAYER_DEFAULT		2

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

		static void Unregister(uint32 layer, bool(fn)(E&))
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
			for (int64 i = s_List.size() - 1; i >= 0; --i) {
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

	template<typename E>
	void Register(uint32 layer, const std::function<bool(E&)>& fn)
	{
		jsh::Listener<E> listener = fn;
		listener.SetLayer(layer);
		jsh::Listener<E>::PushListener(listener);
	}

	template<typename E>
	void Unregister(uint32 layer, bool(fn)(E&))
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

#define JSH_EVENT_PRESSED 1
#define JSH_EVENT_RELEASED 2

#define JSH_EVENT_BUTTON 0
#define JSH_EVENT_DRAGGED 1
#define JSH_EVENT_WHEEL 2

namespace jsh {

	// KEY EVENTS
	class KeyEvent : public jsh::Event {
		uint8 m_Mode = 0u;

	public:
		uint8 keyCode = 0u;

		KeyEvent() : Event(), keyCode(0), m_Mode(0) {}
		KeyEvent(uint8 keyCode, uint8 mode = 0u) : Event(), keyCode(keyCode), m_Mode(mode) {}

		inline bool IsPressed() const noexcept { return m_Mode == JSH_EVENT_PRESSED; }
		inline bool IsReleased() const noexcept { return m_Mode == JSH_EVENT_RELEASED; }
		inline uint8 GetMode() const noexcept { return m_Mode; }
	};

	struct KeyPressedEvent : public jsh::Event {
		uint8 keyCode = 0u;
		KeyPressedEvent(uint8 keyCode) : Event(), keyCode(keyCode) {}
	};

	struct KeyReleasedEvent : public jsh::Event {
		uint8 keyCode = 0u;
		KeyReleasedEvent(uint8 keyCode) : Event(), keyCode(keyCode) {}
	};

	// MOUSE EVENTS
	class MouseEvent : public jsh::Event {
		uint8 m_Mode = 0u;

	public:
		MouseEvent(uint8 mode = 0u) : Event(), m_Mode(mode) {}

		inline bool IsWheel() const noexcept { return m_Mode == JSH_EVENT_WHEEL; }
		inline bool IsButton() const noexcept { return m_Mode == JSH_EVENT_BUTTON; }
		inline bool IsDragged() const noexcept { return m_Mode == JSH_EVENT_DRAGGED; }
		inline uint8 GetMode() const noexcept { return m_Mode; }
	};

	struct MouseDraggedEvent : public jsh::Event {
		float mouseX = 0u;
		float mouseY = 0u;
		float draggedX = 0;
		float draggedY = 0;

		MouseDraggedEvent(float x, float y, float dx, float dy)
			: mouseX(x), mouseY(y), draggedX(dx), draggedY(dy) {}
	};

	struct MouseWheelEvent : public jsh::Event {
		float dragged = 0.f;
		MouseWheelEvent(float d) : dragged(d) {}
	};

	struct MouseButtonEvent : public jsh::Event {
		uint8 m_Mode = 0u;

	public:
		uint8 buttonCode = 0u;

		MouseButtonEvent() : Event(), buttonCode(0), m_Mode(0) {}
		MouseButtonEvent(uint8 keyCode, uint8 mode = 0u) : Event(), buttonCode(keyCode), m_Mode(mode) {}

		inline bool IsPressed() const noexcept { return m_Mode == JSH_EVENT_PRESSED; }
		inline bool IsReleased() const noexcept { return m_Mode == JSH_EVENT_RELEASED; }
		inline uint8 GetMode() const noexcept { return m_Mode; }
	};

	struct MouseButtonPressedEvent : public jsh::Event {
		uint8 buttonCode = 0u;
		MouseButtonPressedEvent(uint8 buttonCode) : Event(), buttonCode(buttonCode) {}
	};

	struct MouseButtonReleasedEvent : public jsh::Event {
		uint8 buttonCode = 0u;
		MouseButtonReleasedEvent(uint8 buttonCode) : Event(), buttonCode(buttonCode) {}
	};

	// WINDOW EVENTS

	struct WindowResizedEvent : public jsh::Event {
		jsh::ivec2 size;
		WindowResizedEvent(int32 width, int32 height) 
			: size(width, height) {}
	};

	struct WindowMovedEvent : public jsh::Event {
		jsh::ivec2 pos;
		WindowMovedEvent(int32 x, int32 y)
			: pos(x, y) {}
	};

	struct WindowGainFocusEvent : public jsh::Event {};
	struct WindowLostFocusEvent : public jsh::Event {};

	// GRAPHICS EVENTS
	struct OutputModeEvent : public jsh::Event {
		OutputMode outputMode;
		OutputModeEvent(const jsh::OutputMode& mode)
			: outputMode(mode) {}
	};

	struct FullscreenEvent : public jsh::Event {
		bool fullscreen;
		FullscreenEvent(bool fullscreen) : fullscreen(fullscreen) {}
	};

	jshEventDefineListener(KeyListener, KeyEvent);
	jshEventDefineListener(KeyPressedListener, KeyPressedEvent);
	jshEventDefineListener(KeyReleasedListener, KeyReleasedEvent);
	jshEventDefineListener(MouseListener, MouseEvent);
	jshEventDefineListener(MouseDraggedListener, MouseDraggedEvent);
	jshEventDefineListener(MouseWheelListener, MouseWheelEvent);
	jshEventDefineListener(MouseButtonListener, MouseButtonEvent);
	jshEventDefineListener(MouseButtonPressedListener, MouseButtonPressedEvent);
	jshEventDefineListener(MouseButtonReleasedListener, MouseButtonReleasedEvent);
	jshEventDefineListener(WindowResizedListener, WindowResizedEvent);
	jshEventDefineListener(WindowMovedListener, WindowMovedEvent);
	jshEventDefineListener(WindowGainFocusListener, WindowGainFocusEvent);
	jshEventDefineListener(WindowLostFocusListener, WindowLostFocusEvent);
	jshEventDefineListener(OutputModeListener, OutputModeEvent);
	jshEventDefineListener(FullscreenListener, FullscreenEvent);

}