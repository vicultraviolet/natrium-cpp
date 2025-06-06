#include "Pch.hpp"
#include "Natrium/Core/Input.hpp"

#if defined(NA_PLATFORM_LINUX) || defined(NA_PLATFORM_WINDOWS)

#include <GLFW/glfw3.h>

#endif // NA_PLATFORM

namespace Na {
	void Input::reset(void)
	{
		memset(this, 0, sizeof(Input) - sizeof(m_GamepadAxes));
		m_GamepadAxes.fill(-1.0f);

		for (Joystick jid = Joysticks::k_1; jid <= Joysticks::k_Last; jid++)
			if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid))
				m_Gamepads.set(jid, true);
	}

	void Input::on_event(const Event& e)
	{
		switch (e.type)
		{
		case EventType::KeyPressed:
            m_Keys.set(e.key_pressed.key, true);
			break;
		case EventType::KeyReleased:
            m_Keys.set(e.key_released.key, false);
			break;
		case EventType::MouseButtonPressed:
			m_MouseButtons.set(e.mouse_button_pressed.button, true);
			break;
		case EventType::MouseButtonReleased:
			m_MouseButtons.set(e.mouse_button_released.button, false);
			break;
		case EventType::MouseMoved:
			m_MousePos.x = e.mouse_moved.x;
			m_MousePos.y = e.mouse_moved.y;
			break;
        case EventType::GamepadConnected:
			m_Gamepads.set(e.gamepad_connected.joystick_id, true);
            break;
        case EventType::GamepadDisconnected:
            m_Gamepads.set(e.gamepad_connected.joystick_id, false);
            break;
        case EventType::GamepadButtonPressed:
            m_GamepadButtons.set(this->_GamepadButtonIndex(e.gamepad_button_pressed.joystick_id, e.gamepad_button_pressed.button), true);
            break;
        case EventType::GamepadButtonReleased:
            m_GamepadButtons.set(this->_GamepadButtonIndex(e.gamepad_button_released.joystick_id, e.gamepad_button_released.button), false);
            break;
        case EventType::GamepadAxisMoved:
			m_GamepadAxes[this->_GamepadAxisIndex(
				e.gamepad_axis_moved.joystick_id,
				e.gamepad_axis_moved.axis
			)] = e.gamepad_axis_moved.value;
            break;
		}
	}

} // namespace Na
