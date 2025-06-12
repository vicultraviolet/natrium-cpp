#if !defined(NA_WINDOW_IMPL_GLFW)
#define NA_WINDOW_IMPL_GLFW

#if defined(NA_PLATFORM_LINUX) || defined(NA_PLATFORM_WINDOWS)

#include "Natrium/Core.hpp"
#include "Natrium/Core/Event.hpp"

struct GLFWwindow;

namespace Na {
	void Window_SetGLFWCallbacks(GLFWwindow* window);
	EventQueue& PollEvents(void);

	class WindowImpl_GLFW {
	public:
		WindowImpl_GLFW(void) = default;
		WindowImpl_GLFW(u32 width, u32 height, const std::string_view& title);

		~WindowImpl_GLFW(void) { this->destroy(); }
		void destroy(void);

		WindowImpl_GLFW(const WindowImpl_GLFW& other) = delete;
		WindowImpl_GLFW& operator=(const WindowImpl_GLFW& other) = delete;

		WindowImpl_GLFW(WindowImpl_GLFW&& other);
		WindowImpl_GLFW& operator=(WindowImpl_GLFW&& other);

		void focus(void);
		void unfocus(void);
		[[nodiscard]] inline bool focused(void) const { return m_Focus; }

		void capture_mouse(void);
		void release_mouse(void);
		[[nodiscard]] inline bool mouse_captured(void) const { return m_MouseCaptured; }

		void minimize(void);
		void restore(void);
		[[nodiscard]] inline bool minimized(void) const { return m_Minimized; }

		[[nodiscard]] inline u32 width(void) const { return m_Width; }
		[[nodiscard]] inline u32 height(void) const { return m_Height; }
		void set_size(u32 width, u32 height);

		void set_title(const char* title);
		[[nodiscard]] const char* title(void) const;

		[[nodiscard]] inline GLFWwindow* native(void) const { return m_Window; }
		inline operator bool(void) const { return m_Window; }
	private:
		friend void Window_SetGLFWCallbacks(GLFWwindow* window);
	private:
		GLFWwindow* m_Window = nullptr;
		u32 m_Width, m_Height;

		bool m_Focus = false;
		bool m_Minimized = false;
		bool m_MouseCaptured = false;
	};

	using Window = WindowImpl_GLFW;
} // namespace Na

#endif // NA_PLATFORM

#endif // NA_WINDOW_IMPL_GLFW