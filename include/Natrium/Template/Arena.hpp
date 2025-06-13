#if !defined(NA_ARENA_HPP)
#define NA_ARENA_HPP

#include "Natrium/Template/ArrayList.hpp"
#include "Natrium/Template/Handles.hpp"

namespace Na {
    template<typename T, typename t_Allocator = std::allocator<T>>
    class Arena {
    public:
        using T_t = T;
        using UniqueHandle = Na::UniqueHandle<Arena>;

        Arena(void) = default;
        ~Arena(void) { this->destroy(); }

        explicit Arena(u64 capacity, const t_Allocator& allocator = t_Allocator())
        : m_Capacity(capacity), m_FreeList(capacity), m_Allocator(allocator)
        {
			m_Buffer = m_Allocator.allocate(capacity);
            for (u64 i = 0; i < capacity; i++)
				m_FreeList.emplace_back(i);
        }

        void clear(void)
        {
            ArrayList<bool> is_free(initialize, m_Capacity, false);
            for (u64 free_index : m_FreeList)
                is_free[free_index] = true;

            for (u64 i = 0; i < m_Capacity; ++i)
                if (!is_free[i])
                    std::destroy_at(m_Buffer + i);

            m_FreeList.clear();
        }

        void destroy(void)
        {
            this->clear();
            m_Allocator.deallocate(m_Buffer, m_Capacity);
            m_Buffer = nullptr;
            m_Capacity = 0;
        }

        Arena(const Arena& other) = delete;
        Arena& operator=(const Arena& other) = delete;

        Arena(Arena&& other) noexcept
        : m_Capacity(std::exchange(other.m_Capacity, 0)),
        m_Buffer(std::exchange(other.m_Buffer, nullptr)),
        m_FreeList(std::move(other.m_FreeList)),
        m_Allocator(std::move(other.m_Allocator))
        {}

        Arena& operator=(Arena&& other) noexcept
        {
            if (this == &other)
                return *this;

            this->destroy();

            m_Capacity = std::exchange(other.m_Capacity, 0);
            m_Buffer = std::exchange(other.m_Buffer, nullptr);
            m_FreeList = std::move(other.m_FreeList);
            m_Allocator = std::move(other.m_Allocator);

            return *this;
        }

        [[nodiscard]] u64 fetch_slot(void)
        {
            if (m_FreeList.empty())
                this->reallocate((u64)std::ceil(m_Capacity * 1.5));

            u64 index = m_FreeList.back();
            m_FreeList.pop_back();
            return index;
        }

        void release_slot(u64 index)
        {
            NA_VERIFY(index < m_Capacity, "Failed to deallocate Arena element: out of bounds!");
            m_FreeList.emplace_back(index);
            std::destroy_at(m_Buffer + index);
        }
		inline void remove_at(u64 index) { this->release_slot(index); }

        template<typename... t_Args>
        [[nodiscard]] UniqueHandle make_unique(t_Args&&... args)
        {
            u64 index = this->fetch_slot();
            if (index == k_InvalidHandle)
                return nullptr;

            std::construct_at(m_Buffer + index, std::forward<t_Args>(args)...);
            return UniqueHandle(this, index);
        }

        void reallocate(u64 new_capacity)
        {
            if (new_capacity == m_Capacity)
                return;

            for (u64 i = new_capacity; i < m_Capacity; i++)
                std::destroy_at(m_Buffer + i);

            T* new_buffer = m_Allocator.allocate(new_capacity);

            for (u64 i = 0; i < std::min(m_Capacity, new_capacity); i++)
                std::construct_at(new_buffer + i, std::move(m_Buffer[i]));

			m_Allocator.deallocate(m_Buffer, m_Capacity);

            m_Buffer = new_buffer;
            m_Capacity = new_capacity;

            m_FreeList.reallocate(new_capacity);
            for (u64 i = m_Capacity; i < new_capacity; i++)
                m_FreeList.emplace_back(i);
        }

        inline void reserve(u64 extra_capacity) { this->reallocate(m_Capacity + extra_capacity); }

        [[nodiscard]] inline T& operator[](u64 index)
        {
            NA_VERIFY(index < m_Capacity, "Failed to index Arena: out of bounds!");
            return m_Buffer[index];
        }
        [[nodiscard]] inline const T& operator[](u64 index) const
        {
            NA_VERIFY(index < m_Capacity, "Failed to index Arena: out of bounds!");
            return m_Buffer[index];
        }

        [[nodiscard]] inline u64 capacity(void) const { return m_Capacity; }
        [[nodiscard]] inline u64 size(void) const { return m_Capacity - m_FreeList.size(); }

        [[nodiscard]] inline bool full(void) const { return m_FreeList.empty(); }
        [[nodiscard]] inline bool empty(void) const { return m_Capacity == 0 || m_FreeList.size() == m_Capacity; }

        [[nodiscard]] inline T* ptr(void) { return m_Buffer; }
        [[nodiscard]] inline const T* ptr(void) const { return m_Buffer; }
    private:
        friend class UniqueHandle;

        u64 m_Capacity = 0;
        T* m_Buffer = nullptr;

        Na::ArrayList<u64> m_FreeList;

        t_Allocator m_Allocator;
    };

    template<typename T, typename t_Allocator = std::allocator<T>>
    using Arena_UniqueHandle = Na::UniqueHandle<Arena<T, t_Allocator>>;
} // namespace Na

#endif // NA_ARENA_HPP