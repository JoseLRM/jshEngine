#pragma once

#include "common.h"
#include <functional>
#include <vector>

#include "GraphicsAdapter.h"

#define JSH_EVENT_LAYER_SYSTEM		0
#define JSH_EVENT_LAYER_GUI			1
#define JSH_EVENT_LAYER_DEFAULT		2

namespace jshEvent {

	class Event {
		bool m_Dead = false;

	public:
		Event() : m_Dead(false) {}

		inline void Kill() noexcept { m_Dead = true; }
		inline bool IsDead() const noexcept { return m_Dead; }
	};

	template<typename E>
	class Listener {

		uint16 m_Layer = JSH_EVENT_LAYER_DEFAULT;
		std::function<bool(E&)> m_Fn;
		bool m_Registred = false;

	public:
		bool operator()(E& e) {
			return m_Fn(e);
		}

		void Unregister()
		{
			if (m_Registred) {
				Dispatcher<E>::Unregister(this);
				m_Registred = false;
			}
		}
		void Register(uint16 layer, const std::function<bool(E&)>& function)
		{
			Unregister();
			m_Fn = std::move(function);
			Dispatcher<E>::Register(this);
			m_Registred = true;
		}
		void Register(const std::function<bool(E&)>& function)
		{
			Register(m_Layer, function);
		}
		~Listener()
		{
			Unregister();
		}

		inline bool operator==(const Listener<E>& other) const
		{
			return this == &other;
		}
		inline bool operator<(const Listener<E>& other) const
		{
			return m_Layer > other.m_Layer;
		}
		inline bool operator>(const Listener<E>& other) const
		{
			return m_Layer < other.m_Layer;
		}
		
		void SetLayer(uint16 layer)
		{
			m_Layer = layer;
			if (m_Fn != nullptr) {
				Register(m_Layer, m_Fn);
			}
		}
		inline uint16 GetLayer() const noexcept { return m_Layer; }

	};

	template<typename E>
	class Dispatcher {
	private:
		static std::vector<Listener<E>*> s_List;

	public:
		static void Register(Listener<E>* listener)
		{
			if (s_List.size() == 0) s_List.emplace_back(listener);
			else {
				auto it = std::lower_bound(s_List.begin(), s_List.end(), listener, [](const Listener<E>* other, const Listener<E>* other1) {
					return (*other).operator<(*other1);
				});
				s_List.insert(it, listener);
			}
		}

		static void Unregister(Listener<E>* listener)
		{
			auto it = std::find(s_List.begin(), s_List.end(), listener);
			if (it != s_List.end()) {
				s_List.erase(it);
			}
		}

		static void Call(E& e)
		{
			for (int32 i = s_List.size() - 1; i >= 0; --i) {
				if (!s_List[i]->operator()(e)) {
					s_List.erase(s_List.begin() + i);
				}
				if (e.IsDead()) break;
			}
		}

	};

	template<typename E>
	std::vector<Listener<E>*> Dispatcher<E>::s_List;

	template<typename E>
	void Dispatch(E& e) {
		Dispatcher<E>::Call(e);
	}

}

#define jshEventDefineListener(dispatcher, e) struct dispatcher : public jshEvent::Dispatcher<e> {}

#define JSH_EVENT_PRESSED 1
#define JSH_EVENT_RELEASED 2

#define JSH_EVENT_BUTTON 0
#define JSH_EVENT_DRAGGED 1
#define JSH_EVENT_WHEEL 2

namespace jsh {

	// KEY EVENTS
	class KeyEvent : public jshEvent::Event {
		uint8 m_Mode = 0u;

	public:
		uint8 keyCode = 0u;

		KeyEvent() : Event(), keyCode(0), m_Mode(0) {}
		KeyEvent(uint8 keyCode, uint8 mode = 0u) : Event(), keyCode(keyCode), m_Mode(mode) {}

		inline bool IsPressed() const noexcept { return m_Mode == JSH_EVENT_PRESSED; }
		inline bool IsReleased() const noexcept { return m_Mode == JSH_EVENT_RELEASED; }
		inline uint8 GetMode() const noexcept { return m_Mode; }
	};

	struct KeyPressedEvent : public jshEvent::Event {
		uint8 keyCode = 0u;
		KeyPressedEvent(uint8 keyCode) : Event(), keyCode(keyCode) {}
	};

	struct KeyReleasedEvent : public jshEvent::Event {
		uint8 keyCode = 0u;
		KeyReleasedEvent(uint8 keyCode) : Event(), keyCode(keyCode) {}
	};

	// MOUSE EVENTS
	class MouseEvent : public jshEvent::Event {
		uint8 m_Mode = 0u;

	public:
		MouseEvent(uint8 mode = 0u) : Event(), m_Mode(mode) {}

		inline bool IsWheel() const noexcept { return m_Mode == JSH_EVENT_WHEEL; }
		inline bool IsButton() const noexcept { return m_Mode == JSH_EVENT_BUTTON; }
		inline bool IsDragged() const noexcept { return m_Mode == JSH_EVENT_DRAGGED; }
		inline uint8 GetMode() const noexcept { return m_Mode; }
	};

	struct MouseDraggedEvent : public jshEvent::Event {
		float mouseX = 0u;
		float mouseY = 0u;
		float draggedX = 0;
		float draggedY = 0;

		MouseDraggedEvent(float x, float y, float dx, float dy)
			: mouseX(x), mouseY(y), draggedX(dx), draggedY(dy) {}
	};

	struct MouseWheelEvent : public jshEvent::Event {
		float dragged = 0.f;
		MouseWheelEvent(float d) : dragged(d) {}
	};

	struct MouseButtonEvent : public jshEvent::Event {
		uint8 m_Mode = 0u;

	public:
		uint8 buttonCode = 0u;

		MouseButtonEvent() : Event(), buttonCode(0), m_Mode(0) {}
		MouseButtonEvent(uint8 keyCode, uint8 mode = 0u) : Event(), buttonCode(keyCode), m_Mode(mode) {}

		inline bool IsPressed() const noexcept { return m_Mode == JSH_EVENT_PRESSED; }
		inline bool IsReleased() const noexcept { return m_Mode == JSH_EVENT_RELEASED; }
		inline uint8 GetMode() const noexcept { return m_Mode; }
	};

	struct MouseButtonPressedEvent : public jshEvent::Event {
		uint8 buttonCode = 0u;
		MouseButtonPressedEvent(uint8 buttonCode) : Event(), buttonCode(buttonCode) {}
	};

	struct MouseButtonReleasedEvent : public jshEvent::Event {
		uint8 buttonCode = 0u;
		MouseButtonReleasedEvent(uint8 buttonCode) : Event(), buttonCode(buttonCode) {}
	};

	// WINDOW EVENTS

	struct WindowResizedEvent : public jshEvent::Event {
		jsh::ivec2 size;
		WindowResizedEvent(int32 width, int32 height) 
			: size(width, height) {}
	};

	struct WindowMovedEvent : public jshEvent::Event {
		jsh::ivec2 pos;
		WindowMovedEvent(int32 x, int32 y)
			: pos(x, y) {}
	};

	struct WindowGainFocusEvent : public jshEvent::Event {};
	struct WindowLostFocusEvent : public jshEvent::Event {};

	// GRAPHICS EVENTS
	struct OutputModeEvent : public jshEvent::Event {
		OutputMode outputMode;
		OutputModeEvent(const jsh::OutputMode& mode)
			: outputMode(mode) {}
	};
	struct ResolutionEvent : public jshEvent::Event {
		uint32 width;
		uint32 height;
		ResolutionEvent(uint32 w, uint32 h) : width(w), height(h) {}
	};
	struct FullscreenEvent : public jshEvent::Event {
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
	jshEventDefineListener(ResolutionListener, ResolutionEvent);
	jshEventDefineListener(FullscreenListener, FullscreenEvent);

}