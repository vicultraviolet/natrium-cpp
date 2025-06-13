#if !defined(NA_HANDLES_HPP)
#define NA_HANDLES_HPP

#include "Natrium/Core.hpp"

namespace Na {
	template<typename t_Container>
	class UniqueHandle {
	public:
		using T = typename t_Container::T_t;

		UniqueHandle(void) = default;
		~UniqueHandle(void) { this->destroy(); }

		void destroy(void)
		{
			if (!*this)
				return;

			m_Container->remove_at(m_Index);

			m_Container = nullptr;
			m_Index = k_InvalidHandle;
		}

		UniqueHandle(nullptr_t) {}
		UniqueHandle& operator=(nullptr_t)
		{
			this->destroy();
			return *this;
		}

		UniqueHandle(const UniqueHandle&) = delete;
		UniqueHandle& operator=(const UniqueHandle&) = delete;

		UniqueHandle(UniqueHandle&& other) noexcept
		: m_Container(std::exchange(other.m_Container, nullptr)),
		m_Index(std::exchange(other.m_Index, k_InvalidHandle))
		{}

		UniqueHandle& operator=(UniqueHandle&& other) noexcept
		{
			if (this == &other)
				return *this;

			this->destroy();

			m_Container = std::exchange(other.m_Container, nullptr);
			m_Index = std::exchange(other.m_Index, k_InvalidHandle);

			return *this;
		}

		UniqueHandle(t_Container* container, u64 index = k_InvalidHandle)
		: m_Container(container), m_Index(index)
		{}

		u64 release(void)
		{
			if (!*this)
				return k_InvalidHandle;

			u64 index = m_Index;

			m_Container = nullptr;
			m_Index = k_InvalidHandle;

			return index;
		}

		T* release(nullptr_t)
		{
			if (!*this)
				return nullptr;

			T* ptr = this->ptr();

			this->destroy();
			return ptr;
		}

		void swap(T& other)
		{
			NA_VERIFY(*this, "Failed to swap UniqueHandle: Container is null or index is invalid!");
			NA_VERIFY(other, "Failed to swap UniqueHandle: Other handle is null or index is invalid!");

			std::swap(m_Container, other.m_Container);
			std::swap(m_Index, other.m_Index);
		}

		template<typename U, std::enable_if_t<std::is_base_of_v<U, T>, int> = 0>
		[[nodiscard]] inline operator UniqueHandle<U>(void)&& { return UniqueHandle<U>(m_Container, this->release()); }

		[[nodiscard]] T* ptr(void)
		{
			NA_VERIFY(*this, "Failed to dereference UniqueHandle: Container is null or index is invalid!");
			return &m_Container->operator[](m_Index);
		}

		[[nodiscard]] const T* ptr(void) const
		{
			NA_VERIFY(*this, "Failed to dereference UniqueHandle: Container is null or index is invalid!");
			return &m_Container->operator[](m_Index);
		}

		[[nodiscard]] inline T& operator*(void) { return *this->ptr(); }
		[[nodiscard]] const T& operator*(void) const { return *this->ptr(); }

		[[nodiscard]] inline T* operator->(void) { return this->ptr(); }
		[[nodiscard]] inline const T* operator->(void) const { return this->ptr(); }

		[[nodiscard]] auto operator==(const UniqueHandle& other) const
		{
			return m_Index == other.m_Index && m_Container == other.m_Container;
		}

		[[nodiscard]] inline u64 index(void) const { return m_Index; }

		[[nodiscard]] operator bool(void) const
		{
			return m_Container && m_Index < m_Container->capacity();
		}
	private:
		t_Container* m_Container = nullptr;
		u64 m_Index = k_InvalidHandle;
	};
} // namespace Na

#endif // NA_HANDLES_HPP