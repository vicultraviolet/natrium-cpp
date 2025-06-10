#if !defined(NA_ARRAY_LIST_HPP)
#define NA_ARRAY_LIST_HPP

#include "./ArrayIterator.hpp"

namespace Na {
	template<typename T>
	class ArrayList {
	public:
		using iterator = Array_Iterator<ArrayList>;
		using reverse_iterator = Array_ReverseIterator<ArrayList>;
		using const_iterator = Array_ConstIterator<ArrayList>;
		using const_reverse_iterator = Array_ConstReverseIterator<ArrayList>;
		using T_t = T;
	public:
		ArrayList(void) = default;
		~ArrayList(void) { this->destroy(); }

		void clear(void)
		{
			for (u64 i = 0; i < m_Size; i++)
				m_Buffer[i].~T();
			m_Size = 0;
		}

		void destroy(void)
		{
			if (!m_Capacity)
				return;

			this->clear();
			free(m_Buffer);

			m_Buffer = nullptr;
			m_Capacity = 0;
			m_Size = 0;
		}

		ArrayList(u64 capacity, u64 size = 0)
		: m_Capacity(capacity), m_Size(size), m_Buffer(tcalloc<T>(capacity))
		{}

		template<typename t_Iterator>
		ArrayList(const t_Iterator& begin, const t_Iterator& end)
		: m_Capacity(std::distance(begin, end)), m_Size(m_Capacity), m_Buffer(tcalloc<T>(m_Size))
		{
			u64 i = 0;
			for (t_Iterator it = begin; it != end; it++)
				new (m_Buffer + i++) T(*it);
		}

		ArrayList(const T* buffer, u64 size)
		: m_Capacity(size), m_Size(size), m_Buffer(tcalloc<T>(size))
		{
			for (u64 i = 0; i < size; i++)
				new (m_Buffer + i) T(buffer[i]);
		}

		ArrayList(const std::initializer_list<T>& list)
		: ArrayList(list.begin(), list.size())
		{}

		ArrayList(const ArrayList& other)
		: ArrayList(other.m_Buffer, other.m_Size)
		{}

		ArrayList& operator=(const ArrayList& other)
		{
			if (this == &other)
				return *this;

			this->clear();

			if (m_Capacity != other.m_Capacity)
			{
				free(m_Buffer);
				m_Buffer = tcalloc<T>(other.m_Capacity);
				m_Capacity = other.m_Capacity;
			}

			for (u64 i = 0; i < other.m_Size; i++)
				new (m_Buffer + i) T(other.m_Buffer[i]);

			return *this;
		}

		ArrayList(ArrayList&& other)
		: m_Buffer(std::exchange(other.m_Buffer, nullptr)),
		m_Capacity(std::exchange(other.m_Capacity, 0)),
		m_Size(std::exchange(other.m_Size, 0))
		{}

		ArrayList& operator=(ArrayList&& other)
		{
			this->clear();
			free(m_Buffer);

			m_Buffer = std::exchange(other.m_Buffer, nullptr);
			m_Capacity = std::exchange(other.m_Capacity, 0);
			m_Size = std::exchange(other.m_Size, 0);

			return *this;
		}

		void resize(u64 new_size)
		{
			NA_ASSERT(m_Capacity >= new_size, "Failed to resize ArrayList: specified size is bigger than capacity!");
			m_Size = new_size;
		}

		void reallocate(u64 new_capacity)
		{
			if (new_capacity == 0)
				return this->destroy();

			if (m_Capacity == new_capacity)
				return;

			T* new_buffer = tcalloc<T>(new_capacity);
			for (u64 i = 0; i < m_Size; i++)
				new (new_buffer + i) T(std::move(m_Buffer[i]));

			free(m_Buffer);
			m_Buffer = new_buffer;
			m_Capacity = new_capacity;
		}

		inline void reserve(u64 extra_capacity) { this->reallocate(m_Capacity + extra_capacity); }

		void reallocate(u64 new_capacity, u64 new_size)
		{
			this->reallocate(new_capacity);
			this->resize(new_size);
		}

		template<typename... t_Args>
		inline u64 emplace(t_Args&&... __args)
		{
			if (m_Size == m_Capacity)
				this->reallocate(m_Capacity * 2 + 1);
			new (m_Buffer + m_Size) T(std::forward<t_Args>(__args)...);
			return m_Size++;
		}

		template<typename... t_Args>
		inline u64 emplace_d(t_Args&&... __args)
		{
			NA_ASSERT(m_Size < m_Capacity, "Failed to emplace to ArrayList: emplace_d called with full buffer!");

			new (m_Buffer + m_Size) T(std::forward<t_Args>(__args)...);
			return m_Size++;
		}

		inline bool pop(void)
		{
			if (m_Size)
				m_Buffer[--m_Size].~T();
			return m_Size;
		}

		[[nodiscard]] inline iterator begin(void) { return m_Buffer; }
		[[nodiscard]] inline const_iterator begin(void) const { return m_Buffer; }
		[[nodiscard]] inline const_iterator cbegin(void) const { return m_Buffer; }

		[[nodiscard]] inline iterator end(void) { return m_Buffer + m_Size; }
		[[nodiscard]] inline const_iterator end(void) const { return m_Buffer + m_Size; }
		[[nodiscard]] inline const_iterator cend(void) const { return m_Buffer + m_Size; }

		[[nodiscard]] inline reverse_iterator rbegin(void) { return m_Buffer + m_Size - 1; }
		[[nodiscard]] inline const_reverse_iterator rbegin(void) const { return m_Buffer + m_Size - 1; }
		[[nodiscard]] inline const_reverse_iterator crbegin(void) const { return m_Buffer + m_Size - 1; }

		[[nodiscard]] inline reverse_iterator rend(void) { return m_Buffer - 1; }
		[[nodiscard]] inline const_reverse_iterator rend(void) const { return m_Buffer - 1; }
		[[nodiscard]] inline const_reverse_iterator crend(void) const { return m_Buffer - 1; }

		[[nodiscard]] inline iterator at(u64 index) { return m_Buffer + index; }
		[[nodiscard]] inline const_iterator at(u64 index) const { return m_Buffer + index; }

		[[nodiscard]] inline T& operator[](u64 index) { return m_Buffer[index]; }
		[[nodiscard]] inline const T& operator[](u64 index) const { return m_Buffer[index]; }

		[[nodiscard]] inline T& operator*(void) { return *m_Buffer; }
		[[nodiscard]] inline const T& operator*(void) const { return *m_Buffer; }

		[[nodiscard]] inline T* operator->(void) { return m_Buffer; }
		[[nodiscard]] inline const T* operator->(void) const { return m_Buffer; }

		[[nodiscard]] inline T* ptr(void) { return m_Buffer; }
		[[nodiscard]] inline const T* ptr(void) const { return m_Buffer; }

		[[nodiscard]] inline u64 capacity(void) const { return m_Capacity; }
		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline u64 free_space(void) const { return m_Capacity - m_Size; }
		[[nodiscard]] inline bool empty(void) const { return !m_Size; }
		[[nodiscard]] inline bool full(void) const { return m_Size == m_Capacity; }
	private:
		u64 m_Capacity = 0;
		u64 m_Size = 0;
		T* m_Buffer = nullptr;
	};
} // namespace Na

#endif // NA_ARRAY_LIST_HPP