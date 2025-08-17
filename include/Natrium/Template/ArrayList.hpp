#if !defined(NA_ARRAY_LIST_HPP)
#define NA_ARRAY_LIST_HPP

#include "./ArrayIterator.hpp"

namespace Na {
	template<typename T>
	concept Dereferenceable = requires(T obj) {
		*obj;
	};

	template<typename T, typename t_Allocator = std::allocator<T>>
	class ArrayList {
	public:
		using allocator_type = t_Allocator;
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
				std::destroy_at(m_Buffer + i);

			m_Size = 0;
		}

		void destroy(void)
		{
			if (!m_Capacity)
				return;

			this->clear();

			if (m_Buffer)
				m_Allocator.deallocate(m_Buffer, m_Capacity);

			m_Buffer = nullptr;
			m_Capacity = 0;
		}

		explicit ArrayList(u64 capacity, u64 size = 0, const t_Allocator& allocator = t_Allocator())
		: m_Capacity(capacity), m_Size(size), m_Allocator(allocator)
		{
			m_Buffer = m_Capacity ? m_Allocator.allocate(m_Capacity) : nullptr;
		}

		template<typename... t_Args>
		ArrayList(init, u64 capacity, t_Args&&... __args)
		: m_Capacity(capacity),m_Size(capacity)
		{
			m_Buffer = m_Capacity ? m_Allocator.allocate(m_Capacity) : nullptr;

			for (u64 i = 0; i < m_Capacity; i++)
				std::construct_at(m_Buffer + i, std::forward<t_Args>(__args)...);
		}

		template<Dereferenceable t_Iterator>
		ArrayList(
			const t_Iterator& begin,
			const t_Iterator& end,
			const t_Allocator& allocator = t_Allocator()
		)
		: m_Capacity(std::distance(begin, end)),
		m_Size(m_Capacity),
		m_Allocator(allocator)
		{
			m_Buffer = m_Capacity ? m_Allocator.allocate(m_Capacity) : nullptr;

			u64 i = 0;
			for (t_Iterator it = begin; it != end; it++)
				std::construct_at(m_Buffer + i++, *it);
		}

		ArrayList(const T* buffer, u64 size, const t_Allocator& allocator = t_Allocator())
		: m_Capacity(size), m_Size(size), m_Allocator(allocator)
		{
			m_Buffer = m_Capacity ? m_Allocator.allocate(m_Capacity) : nullptr;

			for (u64 i = 0; i < m_Size; i++)
				std::construct_at(m_Buffer + i, buffer[i]);
		}

		ArrayList(const std::initializer_list<T>& list, const t_Allocator& allocator = t_Allocator())
		: ArrayList(list.begin(), list.end(), allocator)
		{}

		ArrayList(const ArrayList& other)
		: m_Capacity(other.m_Capacity), m_Size(other.m_Size), m_Allocator(std::allocator_traits<t_Allocator>::select_on_container_copy_construction(other.m_Allocator))
		{
			m_Buffer = m_Capacity ? m_Allocator.allocate(m_Capacity) : nullptr;

			for (u64 i = 0; i < m_Size; i++)
				std::construct_at(m_Buffer + i, other[i]);
		}

		ArrayList& operator=(const ArrayList& other)
		{
			if (this == &other)
				return *this;

			this->clear();

			if (m_Capacity != other.m_Capacity)
			{
				if (m_Buffer)
					m_Allocator.deallocate(m_Buffer, m_Capacity);

				m_Capacity = other.m_Capacity;
				m_Buffer = m_Capacity ? m_Allocator.allocate(m_Capacity) : nullptr;
			}

			m_Size = other.m_Size;
			for (u64 i = 0; i < m_Size; i++)
				std::construct_at(m_Buffer + i, other[i]);

			return *this;
		}

		ArrayList(ArrayList&& other) noexcept
		: m_Capacity(std::exchange(other.m_Capacity, 0)),
		m_Size(std::exchange(other.m_Size, 0)),
		m_Buffer(std::exchange(other.m_Buffer, nullptr)),
		m_Allocator(std::move(other.m_Allocator))
		{}

		ArrayList& operator=(ArrayList&& other) noexcept
		{
			if (this == &other)
				return *this;

			this->destroy();

			m_Capacity = std::exchange(other.m_Capacity, 0);
			m_Size = std::exchange(other.m_Size, 0);
			m_Buffer = std::exchange(other.m_Buffer, nullptr);
			m_Allocator = std::move(other.m_Allocator);

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

			T* new_buffer = m_Allocator.allocate(new_capacity);

			u64 copy_size = std::min(m_Size, new_capacity);
			for (u64 i = 0; i < copy_size; i++)
				std::construct_at(new_buffer + i, std::move_if_noexcept(m_Buffer[i]));

			for (u64 i = 0; i < m_Size; i++)
				std::destroy_at(m_Buffer + i);

			if (m_Buffer)
				m_Allocator.deallocate(m_Buffer, m_Capacity);

			m_Buffer = new_buffer;
			m_Capacity = new_capacity;
			m_Size = copy_size;
		}

		inline void reserve(u64 extra_capacity) { this->reallocate(m_Capacity + extra_capacity); }

		void reallocate(u64 new_capacity, u64 new_size)
		{
			this->reallocate(new_capacity);
			this->resize(new_size);
		}

		template<typename... t_Args>
		u64 emplace(t_Args&&... __args)
		{
			if (m_Size == m_Capacity)
				this->reallocate((u64)std::ceil(m_Capacity * 1.5f) + 1);

			return this->emplace_d(std::forward<t_Args>(__args)...);
		}
		template<typename... t_Args>
		inline u64 emplace_back(t_Args&&... __args) { return this->emplace(std::forward<t_Args>(__args)...); }

		template<typename... t_Args>
		u64 emplace_d(t_Args&&... __args)
		{
			NA_ASSERT(m_Size < m_Capacity, "Failed to emplace to ArrayList: emplace_d called with full buffer!");
			std::construct_at(m_Buffer + m_Size, std::forward<t_Args>(__args)...);
			return m_Size++;
		}

		inline void pop(void) { std::destroy_at(m_Buffer + --m_Size); }
		inline void pop_back(void) { return this->pop(); }

		[[nodiscard]] inline iterator at_s(i64 index) { return m_Buffer + index; }
		[[nodiscard]] inline const_iterator at_s(i64 index) const { return m_Buffer + index; }

		[[nodiscard]] inline iterator at(u64 index) { return m_Buffer + index; }
		[[nodiscard]] inline const_iterator at(u64 index) const { return m_Buffer + index; }

		[[nodiscard]] inline iterator begin(void) { return this->at(0); }
		[[nodiscard]] inline const_iterator begin(void) const { return this->at(0); }
		[[nodiscard]] inline const_iterator cbegin(void) const { return this->at(0); }

		[[nodiscard]] inline iterator end(void) { return this->at(m_Size); }
		[[nodiscard]] inline const_iterator end(void) const { return this->at(m_Size);  }
		[[nodiscard]] inline const_iterator cend(void) const { return this->at(m_Size); }

		[[nodiscard]] inline reverse_iterator rbegin(void) { return this->at_s(m_Size - 1).reverse(); }
		[[nodiscard]] inline const_reverse_iterator rbegin(void) const { return this->at_s(m_Size - 1).reverse(); }
		[[nodiscard]] inline const_reverse_iterator crbegin(void) const { return this->at_s(m_Size - 1).reverse(); }

		[[nodiscard]] inline reverse_iterator rend(void) { return this->at_s(-1).reverse(); }
		[[nodiscard]] inline const_reverse_iterator rend(void) const { return this->at_s(-1).reverse(); }
		[[nodiscard]] inline const_reverse_iterator crend(void) const { return this->at_s(-1).reverse(); }

		[[nodiscard]] inline T& front(void) { return *this->begin(); }
		[[nodiscard]] inline const T& front(void) const { return *this->begin(); }

		[[nodiscard]] inline T& back(void) { return *this->rbegin(); }
		[[nodiscard]] inline const T& back(void) const { return *this->rbegin(); }

		[[nodiscard]] inline T& operator[](u64 index) { return *this->at(index); }
		[[nodiscard]] inline const T& operator[](u64 index) const { return *this->at(index); }

		[[nodiscard]] inline T* ptr(void) { return m_Buffer; }
		[[nodiscard]] inline const T* ptr(void) const { return m_Buffer; }

		[[nodiscard]] inline u64 capacity(void) const { return m_Capacity; }
		[[nodiscard]] inline u64 size(void) const { return m_Size; }
		[[nodiscard]] inline u64 free_space(void) const { return m_Capacity - m_Size; }

		[[nodiscard]] inline bool empty(void) const { return !m_Size; }
		[[nodiscard]] inline bool full(void) const { return m_Size == m_Capacity; }

		[[nodiscard]] const t_Allocator& allocator(void) const { return m_Allocator; }
	private:
		u64 m_Capacity = 0;
		u64 m_Size = 0;
		T* m_Buffer = nullptr;
		t_Allocator m_Allocator;
	};
} // namespace Na

#endif // NA_ARRAY_LIST_HPP