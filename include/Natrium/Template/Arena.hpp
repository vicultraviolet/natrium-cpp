#if !defined(NA_ARENA_HPP)
#define NA_ARENA_HPP

#include "Natrium/Template/ArrayList.hpp"
#include "Natrium/Template/Handles.hpp"

namespace Na {
    template<typename T, typename t_Allocator = std::allocator<T>>
    class Arena {
    public:
        using T_t = T;

        template<typename U = T>
        using ViewHandle = Na::ViewHandle<Arena<T, t_Allocator>, U>;

        template<typename U = T>
        using UniqueHandle = Na::UniqueHandle<Arena<T, t_Allocator>, U>;

        template<typename U = T>
        using SharedHandle = Na::SharedHandle<Arena<T, t_Allocator>, U>;

        template<typename U = T>
        using WeakHandle = Na::WeakHandle<Arena<T, t_Allocator>, U>;

        Arena(void) = default;
        ~Arena(void) { this->destroy(); }

        explicit Arena(u64 capacity, const t_Allocator& allocator = t_Allocator())
        : m_Capacity(capacity), m_FreeList(capacity, capacity), m_Allocator(allocator)
        {
			m_Buffer = m_Allocator.allocate(capacity);

            for (u64 i = m_Capacity; i-- > 0; )
                m_FreeList[m_Capacity - 1 - i] = i;
        }

        void clear(void)
        {
            ArrayList<bool> is_free(initialize, m_Capacity, false);
            for (u64 free_index : m_FreeList)
                is_free[free_index] = true;

            for (u64 i = 0; i < m_Capacity; i++)
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
        {
            m_Capacity = other.m_Capacity;
            m_Buffer = other.m_Buffer;
            m_FreeList = std::move(other.m_FreeList);
            m_Allocator = std::move(other.m_Allocator);

            other.m_Capacity = 0;
            other.m_Buffer = nullptr;
        }

        Arena& operator=(Arena&& other) noexcept
        {
            if (this == &other)
                return *this;

            this->destroy();

            m_Capacity = other.m_Capacity;
            m_Buffer = other.m_Buffer;
            m_FreeList = std::move(other.m_FreeList);
            m_Allocator = std::move(other.m_Allocator);

            other.m_Capacity = 0;
            other.m_Buffer = nullptr;

            return *this;
        }

        [[nodiscard]] u64 fetch_slot(void)
        {
            if (m_FreeList.empty())
                this->reallocate((u64)std::ceil(m_Capacity * 1.5f) + 1);

            u64 index = m_FreeList.back();
            m_FreeList.pop_back();
            return index;
        }

        void release_slot(u64 index)
        {
            NA_ASSERT(index < m_Capacity, "Failed to deallocate Arena element: out of bounds!");
            m_FreeList.emplace_back(index);
        }

        template<typename... t_Args>
        [[nodiscard]] u64 emplace(t_Args&&... args)
        {
            u64 index = this->fetch_slot();

            std::construct_at(m_Buffer + index, std::forward<t_Args>(args)...);
            return index;
        }

        inline void remove_at(u64 index)
        {
            this->release_slot(index);
            std::destroy_at(m_Buffer + index);
        }

        template<typename... t_Args>
        [[nodiscard]] ViewHandle<T> make_view(t_Args&&... args)
        {
            return ViewHandle<T>(static_cast<Arena<T, t_Allocator>*>(this), this->emplace(std::forward<t_Args>(args)...));
        }

        template<typename... t_Args>
        [[nodiscard]] UniqueHandle<T> make_unique(t_Args&&... args)
        {
            return UniqueHandle<T>(static_cast<Arena<T, t_Allocator>*>(this), this->emplace(std::forward<t_Args>(args)...));
        }

        template<typename... t_Args>
        [[nodiscard]] SharedHandle<T> make_shared(t_Args&&... args)
        {
            return SharedHandle<T>(static_cast<Arena<T, t_Allocator>*>(this), this->emplace(std::forward<t_Args>(args)...));
        }

        void reallocate(u64 new_capacity)
        {
            if (new_capacity == m_Capacity)
                return;

            u64 old_capacity = m_Capacity;

            ArrayList<bool> is_free(initialize, old_capacity, false);
            for (u64 free_index : m_FreeList)
                is_free[free_index] = true;

            for (u64 i = new_capacity; i < old_capacity; i++)
                if (!is_free[i])
                    std::destroy_at(m_Buffer + i);

            T* new_buffer = m_Allocator.allocate(new_capacity);

            for (u64 i = 0; i < std::min(old_capacity, new_capacity); i++)
            {
                if (!is_free[i])
                {
                    std::construct_at(new_buffer + i, std::move(m_Buffer[i]));
                    std::destroy_at(m_Buffer + i);
                }
            }

            m_Allocator.deallocate(m_Buffer, old_capacity);

            m_Buffer = new_buffer;
            m_Capacity = new_capacity;

            Na::ArrayList<u64> new_free_list(new_capacity);

            u64 free_count = 0;
            for (u64 i = new_capacity; i-- > 0; )
            {
                if ((i < old_capacity && is_free[i]) || (i >= old_capacity))
                    new_free_list[free_count++] = i;
            }
            new_free_list.resize(free_count);

            m_FreeList = std::move(new_free_list);
        }

        inline void reserve(u64 extra_capacity) { this->reallocate(m_Capacity + extra_capacity); }

        [[nodiscard]] inline T& operator[](u64 index)
        {
            NA_ASSERT(index < m_Capacity, "Failed to index Arena: out of bounds!");
            return m_Buffer[index];
        }
        [[nodiscard]] inline const T& operator[](u64 index) const
        {
            NA_ASSERT(index < m_Capacity, "Failed to index Arena: out of bounds!");
            return m_Buffer[index];
        }

        [[nodiscard]] inline u64 capacity(void) const { return m_Capacity; }
        [[nodiscard]] inline u64 size(void) const { return m_Capacity - m_FreeList.size(); }

        [[nodiscard]] inline bool full(void) const { return m_FreeList.empty(); }
        [[nodiscard]] inline bool empty(void) const { return m_Capacity == 0 || m_FreeList.size() == m_Capacity; }

        [[nodiscard]] inline const ArrayList<u64>& free_list(void) const { return m_FreeList; }

        [[nodiscard]] inline T* ptr(void) { return m_Buffer; }
		[[nodiscard]] inline const T* ptr(void) const { return m_Buffer; }
    private:
        u64 m_Capacity = 0;
        T* m_Buffer = nullptr;

        Na::ArrayList<u64> m_FreeList;

        t_Allocator m_Allocator;
    };
} // namespace Na

#endif // NA_ARENA_HPP