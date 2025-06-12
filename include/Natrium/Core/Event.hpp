#if !defined(NA_EVENT_HPP)
#define NA_EVENT_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Template/ArrayList.hpp"
#include "Natrium/Core/InputConstants.hpp"

#define NA_EVENT_BASE(x) struct {\
						Na::EventType type = Na::EventType::x;\
						bool handled = false;\
						Na::Window* window;\
					 }

namespace Na {
#if defined(NA_PLATFORM_LINUX) || defined(NA_PLATFORM_WINDOWS)
	class WindowImpl_GLFW;
	using Window = WindowImpl_GLFW;
#endif // NA_PLATFORM

	enum class EventType : u8 {
		None = 0,
		KeyPressed, KeyReleased,
		CharInput,
		WindowResized, WindowClosed, WindowFocused, WindowLostFocus, WindowMinimized, WindowRestored,
		MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased,
		GamepadConnected, GamepadDisconnected,
		GamepadButtonPressed, GamepadButtonReleased, GamepadAxisMoved
	};

	struct Event_KeyPressed {
		NA_EVENT_BASE(KeyPressed);
		Key key;
		KeyMod mod;
		bool repeat;
		u32 padding = 0;
	};

	struct Event_KeyReleased {
		NA_EVENT_BASE(KeyReleased);
		Key key;
		KeyMod mod;
		u32 padding = 0;
	};

	struct Event_CharInput {
		NA_EVENT_BASE(CharInput);
		char32_t codepoint = 0;
	};

	struct Event_WindowResized {
		NA_EVENT_BASE(WindowResized);
		u32 width, height;
	};

	struct Event_WindowClosed {
		NA_EVENT_BASE(WindowClosed);
		u32 padding1 = 0, padding2 = 0;
	};

	struct Event_WindowFocused {
		NA_EVENT_BASE(WindowFocused);
		u32 padding1 = 0, padding2 = 0;
	};
	struct Event_WindowLostFocus {
		NA_EVENT_BASE(WindowLostFocus);
		u32 padding1 = 0, padding2 = 0;
	};

	struct Event_WindowMinimized {
		NA_EVENT_BASE(WindowMinimized);
		u32 padding1 = 0, padding2 = 0;
	};
	struct Event_WindowRestored {
		NA_EVENT_BASE(WindowRestored);
		u32 padding1 = 0, padding2 = 0;
	};

	struct Event_MouseMoved {
		NA_EVENT_BASE(MouseMoved);
		float x, y;
	};

	struct Event_MouseScrolled {
		NA_EVENT_BASE(MouseScrolled);
		float x_offset, y_offset;
	};

	struct Event_MouseButtonPressed {
		NA_EVENT_BASE(MouseButtonPressed);
		MouseButton button;
		u32 padding = 0;
	};
	struct Event_MouseButtonReleased {
		NA_EVENT_BASE(MouseButtonReleased);
		MouseButton button;
		u32 padding = 0;
	};

	struct Event_GamepadConnected {
		NA_EVENT_BASE(GamepadConnected);
		Joystick joystick_id;
		u32 padding = 0;
	};

	struct Event_GamepadDisconnected {
		NA_EVENT_BASE(GamepadDisconnected);
		Joystick joystick_id;
		u32 padding = 0;
	};

	struct Event_GamepadButtonPressed {
		NA_EVENT_BASE(GamepadButtonPressed);
		Joystick joystick_id;
		GamepadButton button;
		u32 padding = 0;
	};

	struct Event_GamepadButtonReleased {
		NA_EVENT_BASE(GamepadButtonReleased);
		Joystick joystick_id;
		GamepadButton button;
		u32 padding = 0;
	};

	struct Event_GamepadAxisMoved {
		NA_EVENT_BASE(GamepadAxisMoved);
		Joystick joystick_id;
		GamepadAxis axis;
		float value;
	};

	union Event {
		struct {
			EventType type;
			bool handled;
			Window* window;
			u32 padding1, padding2;
		};

		Event_KeyPressed key_pressed;
		Event_KeyReleased key_released;

		Event_CharInput char_input;

		Event_WindowResized window_resized;
		Event_WindowClosed window_closed;

		Event_WindowFocused window_focused;
		Event_WindowLostFocus window_lost_focus;

		Event_WindowMinimized window_minimized;
		Event_WindowRestored window_restored;

		Event_MouseMoved mouse_moved;
		Event_MouseScrolled mouse_scrolled;

		Event_MouseButtonPressed mouse_button_pressed;
		Event_MouseButtonReleased mouse_button_released;

		Event_GamepadConnected gamepad_connected;
		Event_GamepadDisconnected gamepad_disconnected;

		Event_GamepadButtonPressed gamepad_button_pressed;
		Event_GamepadButtonReleased gamepad_button_released;

		Event_GamepadAxisMoved gamepad_axis_moved;
	};

	using EventQueue = ArrayList<Event>;
} // namespace Na

#endif // NA_EVENT_HPP