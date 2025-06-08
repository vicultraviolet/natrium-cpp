#include "Pch.hpp"
#include "Natrium/Platform/Desktop/WindowImpl_GLFW.hpp"

#if defined(NA_PLATFORM_LINUX) || defined(NA_PLATFORM_WINDOWS)

#include "Natrium/Core/Event.hpp"
#include "Natrium/Core/Context.hpp"

#include <GLFW/glfw3.h>

namespace Na {
	static std::unordered_map<Joystick, GLFWgamepadstate> previousGamepadStates;

	EventQueue& PollEvents(void)
	{
		Context::Get().event_queue().resize(0);
		glfwPollEvents();

		for (Joystick jid = Joysticks::k_1; jid <= Joysticks::k_Last; jid++)
		{
			if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid))
			{
				GLFWgamepadstate current_state;
				if (glfwGetGamepadState(jid, &current_state))
				{
					auto& previous_state = previousGamepadStates[jid];
					
					for (GamepadButton button = 0; button <= GamepadButtons::k_Last; button++)
					{
						if (current_state.buttons[button] == previous_state.buttons[button])
							continue;

						if (current_state.buttons[button] == InputActions::k_Press)
							Context::Get().event_queue().emplace(Event{.gamepad_button_pressed = {
								EventType::GamepadButtonPressed,
								false,
								nullptr,
								jid,
								button
						    }});
						else
							Context::Get().event_queue().emplace(Event{.gamepad_button_released = {
								EventType::GamepadButtonReleased,
								false,
								nullptr,
								jid,
								button
							}});
					}

					for (GamepadAxis axis = 0; axis <= GamepadAxes::k_Last; ++axis)
					{
						if (current_state.axes[axis] == previous_state.axes[axis])
							continue;

						Context::Get().event_queue().emplace(Event{.gamepad_axis_moved = {
							EventType::GamepadAxisMoved,
							false,
							nullptr,
							jid,
							axis,
							current_state.axes[axis]
						}});
					}

					previous_state = current_state;
				}
			}
		}

		return Context::Get().event_queue();
	}

	Window::WindowImpl_GLFW(u32 width, u32 height, const std::string_view& title)
	: m_Width(width), m_Height(height)
	{
		m_Window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
		NA_ASSERT(m_Window, "Failed to create glfw window!");

		glfwSetWindowUserPointer(m_Window, this);

		Window_SetGLFWCallbacks(m_Window);

		m_Focus = glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) != 0;
	}

	void Window::destroy(void)
	{
		if (!m_Window)
			return;

		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}

	void Window::focus(void)
	{
		glfwFocusWindow(m_Window);
		m_Focus = true;
	}

	void Window::unfocus(void)
	{
		glfwFocusWindow(nullptr);
		m_Focus = false;
	}

	void Window::capture_mouse(void)
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_MouseCaptured = true;
	}

	void Window::release_mouse(void)
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_MouseCaptured = false;
	}

	void Window::minimize(void)
	{
		glfwIconifyWindow(m_Window);
		m_Minimized = true;
	}

	void Window::restore(void)
	{
		glfwRestoreWindow(m_Window);
		m_Minimized = false;
	}

	void Window::set_size(u32 width, u32 height)
	{
		glfwSetWindowSize(m_Window, m_Width = width, m_Height = height);
	}

	void Window::set_title(const char* title)
	{
		glfwSetWindowTitle(m_Window, title);
	}

	const char* Window::title(void) const
	{
		return glfwGetWindowTitle(m_Window);
	}

	WindowImpl_GLFW::WindowImpl_GLFW(WindowImpl_GLFW&& other)
	: m_Window(std::exchange(other.m_Window, nullptr)),
	m_Width(std::exchange(other.m_Width, 0)),
	m_Height(std::exchange(other.m_Height, 0)),
	m_Focus(std::exchange(other.m_Focus, false)),
	m_Minimized(std::exchange(other.m_Minimized, false)),
	m_MouseCaptured(std::exchange(other.m_MouseCaptured, false))
	{
		glfwSetWindowUserPointer(m_Window, this);
	}

	WindowImpl_GLFW& WindowImpl_GLFW::operator=(WindowImpl_GLFW&& other)
	{
		NA_VERIFY(m_Window != other.m_Window, "Failed to move Window: Cannot move assign a window to itself!");

		if (m_Window)
			glfwDestroyWindow(m_Window);

		m_Window = std::exchange(other.m_Window, nullptr);
		m_Width = std::exchange(other.m_Width, 0);
		m_Height = std::exchange(other.m_Height, 0);
		m_Focus = std::exchange(other.m_Focus, false);
		m_Minimized = std::exchange(other.m_Minimized, false);
		m_MouseCaptured = std::exchange(other.m_MouseCaptured, false);

		glfwSetWindowUserPointer(m_Window, this);

		return *this;
	}

	void Window_SetGLFWCallbacks(GLFWwindow* _window)
	{
		glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case InputActions::k_Press:
				Context::Get().event_queue().emplace(Event{.key_pressed = {
					EventType::KeyPressed,
					false,
					__window,
					(Key)key,
					(KeyMod)mods,
					false
				}});
				break;
			case InputActions::k_Repeat:
				Context::Get().event_queue().emplace(Event{.key_pressed = {
					EventType::KeyPressed,
					false,
					__window,
					(Key)key,
					(KeyMod)mods,
					true
				}});
				break;
			case InputActions::k_Release:
				Context::Get().event_queue().emplace(Event{.key_released = {
					EventType::KeyReleased,
					false,
					__window,
					(Key)key,
					(KeyMod)mods
				}});
				break;
			}
		});
		glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			__window->m_Width = width;
			__window->m_Height = height;
			Context::Get().event_queue().emplace(Event{.window_resized = {
				EventType::WindowResized,
				false,
				__window,
				(u32)width, (u32)height
			}});
		});
		glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			Context::Get().event_queue().emplace(Event{.window_closed = {
				EventType::WindowClosed,
				false,
				__window
			}});
		});
		glfwSetWindowFocusCallback(_window, [](GLFWwindow* window, int focus)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			__window->m_Focus = focus;
			if (focus)
				Context::Get().event_queue().emplace(Event{.window_focused = {
					EventType::WindowFocused,
					false,
					__window
				}});
			else
				Context::Get().event_queue().emplace(Event{.window_lost_focus = {
					EventType::WindowLostFocus,
					false,
					__window
				}});
		});
		glfwSetWindowIconifyCallback(_window, [](GLFWwindow* window, int iconified)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			__window->m_Minimized = iconified;
			if (iconified)
				Context::Get().event_queue().emplace(Event{.window_minimized = {
					EventType::WindowMinimized,
					false,
					__window
				}});
			else
				__window->m_Minimized = false;
				Context::Get().event_queue().emplace(Event{.window_restored = {
					EventType::WindowRestored,
					false,
					__window
				}});
		});
		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double x, double y)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			Context::Get().event_queue().emplace(Event{.mouse_moved = {
				EventType::MouseMoved,
				false,
				__window,
				(float)x, (float)y
			}});
		});
		glfwSetScrollCallback(_window, [](GLFWwindow* window, double x_offset, double y_offset)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			Context::Get().event_queue().emplace(Event{.mouse_scrolled = {
				EventType::MouseScrolled,
				false,
				__window,
				(float)x_offset, (float)y_offset
			}});
		});
		glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			Window* __window = (Window*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case InputActions::k_Press:
				Context::Get().event_queue().emplace(Event{.mouse_button_pressed = {
					EventType::MouseButtonPressed,
					false,
					__window,
					(MouseButton)button
				}});
				break;
			case InputActions::k_Release:
				Context::Get().event_queue().emplace(Event{.mouse_button_released = {
					EventType::MouseButtonReleased,
					false,
					__window,
					(MouseButton)button
				}});
				break;
			}
		});
		glfwSetJoystickCallback([](int jid, int event)
		{
			if (!glfwJoystickIsGamepad(jid))
				return;

			if (event == GLFW_CONNECTED)
			{
				Context::Get().event_queue().emplace(Event{.gamepad_connected = {
					EventType::GamepadConnected,
					false,
					nullptr,
					(Joystick)jid
				}});
			} else
			if (event == GLFW_DISCONNECTED)
			{
				Context::Get().event_queue().emplace(Event{.gamepad_disconnected = {
					EventType::GamepadDisconnected,
					false,
					nullptr,
					(Joystick)jid
				}});
				previousGamepadStates.erase(jid);
			}
		});
	}
} // namespace Na

#endif // NA_PLATFORM
