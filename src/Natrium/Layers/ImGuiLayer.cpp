#include "Pch.hpp"
#include "Natrium/Layers/ImGuiLayer.hpp"

#if !defined(NA_DISABLE_IMGUI)

#include "Natrium/Graphics/VulkanImpl/vImGuiLayer.hpp"

#include <imgui/backends/imgui_impl_glfw.h>

namespace Na {
    ImGuiKey TranslateToImGuiKey(Key key)
    {
        using namespace Na::Keys;
        switch (key)
        {
        case k_A: return ImGuiKey_A;
        case k_B: return ImGuiKey_B;
        case k_C: return ImGuiKey_C;
        case k_D: return ImGuiKey_D;
        case k_E: return ImGuiKey_E;
        case k_F: return ImGuiKey_F;
        case k_G: return ImGuiKey_G;
        case k_H: return ImGuiKey_H;
        case k_I: return ImGuiKey_I;
        case k_J: return ImGuiKey_J;
        case k_K: return ImGuiKey_K;
        case k_L: return ImGuiKey_L;
        case k_M: return ImGuiKey_M;
        case k_N: return ImGuiKey_N;
        case k_O: return ImGuiKey_O;
        case k_P: return ImGuiKey_P;
        case k_Q: return ImGuiKey_Q;
        case k_R: return ImGuiKey_R;
        case k_S: return ImGuiKey_S;
        case k_T: return ImGuiKey_T;
        case k_U: return ImGuiKey_U;
        case k_V: return ImGuiKey_V;
        case k_W: return ImGuiKey_W;
        case k_X: return ImGuiKey_X;
        case k_Y: return ImGuiKey_Y;
        case k_Z: return ImGuiKey_Z;

        case k_0: return ImGuiKey_0;
        case k_1: return ImGuiKey_1;
        case k_2: return ImGuiKey_2;
        case k_3: return ImGuiKey_3;
        case k_4: return ImGuiKey_4;
        case k_5: return ImGuiKey_5;
        case k_6: return ImGuiKey_6;
        case k_7: return ImGuiKey_7;
        case k_8: return ImGuiKey_8;
        case k_9: return ImGuiKey_9;

        case k_F1: return ImGuiKey_F1;
        case k_F2: return ImGuiKey_F2;
        case k_F3: return ImGuiKey_F3;
        case k_F4: return ImGuiKey_F4;
        case k_F5: return ImGuiKey_F5;
        case k_F6: return ImGuiKey_F6;
        case k_F7: return ImGuiKey_F7;
        case k_F8: return ImGuiKey_F8;
        case k_F9: return ImGuiKey_F9;
        case k_F10: return ImGuiKey_F10;
        case k_F11: return ImGuiKey_F11;
        case k_F12: return ImGuiKey_F12;
        case k_F13: return ImGuiKey_F13;
        case k_F14: return ImGuiKey_F14;
        case k_F15: return ImGuiKey_F15;
        case k_F16: return ImGuiKey_F16;
        case k_F17: return ImGuiKey_F17;
        case k_F18: return ImGuiKey_F18;
        case k_F19: return ImGuiKey_F19;
        case k_F20: return ImGuiKey_F20;
        case k_F21: return ImGuiKey_F21;
        case k_F22: return ImGuiKey_F22;
        case k_F23: return ImGuiKey_F23;
        case k_F24: return ImGuiKey_F24;

        case k_KP0: return ImGuiKey_Keypad0;
        case k_KP1: return ImGuiKey_Keypad1;
        case k_KP2: return ImGuiKey_Keypad2;
        case k_KP3: return ImGuiKey_Keypad3;
        case k_KP4: return ImGuiKey_Keypad4;
        case k_KP5: return ImGuiKey_Keypad5;
        case k_KP6: return ImGuiKey_Keypad6;
        case k_KP7: return ImGuiKey_Keypad7;
        case k_KP8: return ImGuiKey_Keypad8;
        case k_KP9: return ImGuiKey_Keypad9;
        case k_KPDecimal: return ImGuiKey_KeypadDecimal;
        case k_KPDivide: return ImGuiKey_KeypadDivide;
        case k_KPMultiply: return ImGuiKey_KeypadMultiply;
        case k_KPSubtract: return ImGuiKey_KeypadSubtract;
        case k_KPAdd: return ImGuiKey_KeypadAdd;
        case k_KPEnter: return ImGuiKey_KeypadEnter;
        case k_KPEqual: return ImGuiKey_KeypadEqual;

        case k_Left: return ImGuiKey_LeftArrow;
        case k_Right: return ImGuiKey_RightArrow;
        case k_Up: return ImGuiKey_UpArrow;
        case k_Down: return ImGuiKey_DownArrow;

        case k_LeftShift: return ImGuiKey_LeftShift;
        case k_RightShift: return ImGuiKey_RightShift;
        case k_LeftControl: return ImGuiKey_LeftCtrl;
        case k_RightControl: return ImGuiKey_RightCtrl;
        case k_LeftAlt: return ImGuiKey_LeftAlt;
        case k_RightAlt: return ImGuiKey_RightAlt;
        case k_LeftSuper: return ImGuiKey_LeftSuper;
        case k_RightSuper: return ImGuiKey_RightSuper;
        case k_Menu: return ImGuiKey_Menu;

        case k_Space: return ImGuiKey_Space;
        case k_Enter: return ImGuiKey_Enter;
        case k_Escape: return ImGuiKey_Escape;
        case k_Tab: return ImGuiKey_Tab;
        case k_Backspace: return ImGuiKey_Backspace;
        case k_Insert: return ImGuiKey_Insert;
        case k_Delete: return ImGuiKey_Delete;
        case k_Home: return ImGuiKey_Home;
        case k_End: return ImGuiKey_End;
        case k_PageUp: return ImGuiKey_PageUp;
        case k_PageDown: return ImGuiKey_PageDown;
        case k_CapsLock: return ImGuiKey_CapsLock;
        case k_ScrollLock: return ImGuiKey_ScrollLock;
        case k_NumLock: return ImGuiKey_NumLock;
        case k_PrintScreen: return ImGuiKey_PrintScreen;
        case k_Pause: return ImGuiKey_Pause;

        case k_Apostrophe: return ImGuiKey_Apostrophe;
        case k_Comma: return ImGuiKey_Comma;
        case k_Minus: return ImGuiKey_Minus;
        case k_Period: return ImGuiKey_Period;
        case k_Slash: return ImGuiKey_Slash;
        case k_Semicolon: return ImGuiKey_Semicolon;
        case k_Equal: return ImGuiKey_Equal;
        case k_LeftBracket: return ImGuiKey_LeftBracket;
        case k_Backslash: return ImGuiKey_Backslash;
        case k_RightBracket: return ImGuiKey_RightBracket;
        case k_GraveAccent: return ImGuiKey_GraveAccent;

        case k_World1: return ImGuiKey_Oem102;
        case k_World2: return ImGuiKey_Oem102;
        }
        return ImGuiKey_None;
    }

	UniqueRef<ImGuiLayer> ImGuiLayer::Make(
		View<const Graphics::Renderer> renderer,
		i64 priority,
		bool demo_window_shown
	)
	{
		switch (Device::Get().backend())
		{
		case DeviceBackend::Vulkan: return UniqueRef<VulkanImpl::ImGuiLayer>::Make(renderer, priority, demo_window_shown);
		}
        return nullptr;
	}

    ImGuiLayer::ImGuiLayer(View<const Graphics::Renderer> renderer, i64 priority, bool demo_window_shown)
    : Layer(priority), m_Renderer(renderer), m_DemoWindowShown(demo_window_shown)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(renderer->window().native(), false);
    }

    ImGuiLayer::~ImGuiLayer(void)
    {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
	}

    void ImGuiLayer::on_event(Event& e)
    {
        ImGuiIO& io = ImGui::GetIO();

        e.handled = io.WantCaptureMouse;

        switch (e.type)
        {
        case EventType::MouseButtonPressed:
            io.AddMouseButtonEvent(
                (int)e.mouse_button_pressed.button,
                true
            );
            break;
        case EventType::MouseButtonReleased:
            io.AddMouseButtonEvent(
                (int)e.mouse_button_pressed.button,
                false
            );
            break;
        case EventType::MouseMoved:
            io.AddMousePosEvent(e.mouse_moved.x, e.mouse_moved.y);
            break;
        case EventType::MouseScrolled:
            io.AddMouseWheelEvent(e.mouse_scrolled.x_offset, e.mouse_scrolled.y_offset);
            break;
        case EventType::KeyPressed:
            io.AddKeyEvent(
                TranslateToImGuiKey(e.key_pressed.key),
                true
            );
            break;
        case EventType::KeyReleased:
            io.AddKeyEvent(
                TranslateToImGuiKey(e.key_pressed.key),
                false
            );
            break;
        case EventType::CharInput:
            io.AddInputCharacter(e.char_input.codepoint);
            break;
        case EventType::WindowResized:
            io.DisplaySize = ImVec2((float)e.window_resized.width, (float)e.window_resized.height);
            break;
        case EventType::WindowFocused:
            io.AddFocusEvent(true);
            break;
        case EventType::WindowLostFocus:
            io.AddFocusEvent(false);
            break;
        case EventType::WindowClosed:
            io.AddFocusEvent(false);
            break;
        }
    }

    void ImGuiLayer::imgui_draw(void)
    {
        if (m_DemoWindowShown)
            ImGui::ShowDemoWindow(&m_DemoWindowShown);
    }
} // namespace Na

#endif // NA_DISABLE_IMGUI
