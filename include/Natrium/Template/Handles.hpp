#if !defined(NA_HANDLES_HPP)
#define NA_HANDLES_HPP

#include "Natrium/Core.hpp"

namespace Na {
    template<typename t_Container, typename T>
    class ViewHandle {
    public:
        ViewHandle(void) = default;
        ~ViewHandle(void) = default;

        void destroy(void)
        {
            if (!*this)
                return;

            m_Container = nullptr;
            m_Index = k_InvalidHandle;
        }

        ViewHandle(nullptr_t) : m_Container(nullptr) {}
        ViewHandle& operator=(nullptr_t)
        {
            this->destroy();
            return *this;
        }

        ViewHandle(const ViewHandle& other)
        : m_Container(other.m_Container), m_Index(other.m_Index)
        {}

        ViewHandle& operator=(const ViewHandle& other)
        {
            m_Container = other.m_Container;
            m_Index = other.m_Index;
        }

        ViewHandle(ViewHandle&& other) noexcept
        : m_Container(std::exchange(other.m_Container, nullptr)),
        m_Index(std::exchange(other.m_Index, k_InvalidHandle))
        {}

        ViewHandle& operator=(ViewHandle&& other) noexcept
        {
            if (this == &other)
                return *this;

            m_Container = std::exchange(other.m_Container, nullptr);
            m_Index = std::exchange(other.m_Index, k_InvalidHandle);

            return *this;
        }

        ViewHandle(t_Container* container, u64 index = k_InvalidHandle)
        : m_Container(container), m_Index(index)
        {}

        void swap(ViewHandle& other)
        {
            std::swap(m_Container, other.m_Container);
            std::swap(m_Index, other.m_Index);
        }

        [[nodiscard]] T* ptr(void)
        {
            NA_ASSERT(*this, "Failed to dereference ViewHandle: Container is null or index is invalid!");
            return &m_Container->operator[](m_Index);
        }

        [[nodiscard]] const T* ptr(void) const
        {
            NA_ASSERT(*this, "Failed to dereference ViewHandle: Container is null or index is invalid!");
            return &m_Container->operator[](m_Index);
        }

        [[nodiscard]] inline T& operator*(void) { return *this->ptr(); }
        [[nodiscard]] const T& operator*(void) const { return *this->ptr(); }

        [[nodiscard]] inline T* operator->(void) { return this->ptr(); }
        [[nodiscard]] inline const T* operator->(void) const { return this->ptr(); }

        template<typename U> requires std::is_base_of_v<U, T>
        operator ViewHandle<t_Container, U>(void) const
        {
            return ViewHandle<t_Container, U>(m_Container, m_Index);
        }

        [[nodiscard]] auto operator==(const ViewHandle& other) const
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

        template<typename To, typename C, typename From>
        friend ViewHandle<C, To> dynamic_handle_cast(const ViewHandle<C, From>&);
    };

    template<typename To, typename t_Container, typename From>
    ViewHandle<t_Container, To> dynamic_handle_cast(const ViewHandle<t_Container, From>& handle)
    {
        if (!handle)
            return nullptr;

        using FromPtr = decltype(handle.ptr());
        using ToPtr = std::conditional_t<
            std::is_const_v<std::remove_pointer_t<FromPtr>>,
            const To*,
            To*
        >;

        if (auto casted = dynamic_cast<ToPtr>(handle.ptr()))
            return ViewHandle<t_Container, To>(handle.m_Container, handle.m_Index);

        return nullptr;
    }

    template<typename t_Container, typename T>
	class UniqueHandle {
	public:
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

		UniqueHandle(nullptr_t) : m_Container(nullptr) {}
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

		void swap(UniqueHandle& other)
		{
			std::swap(m_Container, other.m_Container);
			std::swap(m_Index, other.m_Index);
		}

		[[nodiscard]] T* ptr(void)
		{
            NA_ASSERT(*this, "Failed to dereference UniqueHandle: Container is null or index is invalid!");
			return &m_Container->operator[](m_Index);
		}

		[[nodiscard]] const T* ptr(void) const
		{
            NA_ASSERT(*this, "Failed to dereference UniqueHandle: Container is null or index is invalid!");
			return &m_Container->operator[](m_Index);
		}

		[[nodiscard]] inline T& operator*(void) { return *this->ptr(); }
		[[nodiscard]] const T& operator*(void) const { return *this->ptr(); }

		[[nodiscard]] inline T* operator->(void) { return this->ptr(); }
		[[nodiscard]] inline const T* operator->(void) const { return this->ptr(); }

        [[nodiscard]] inline u64 index(void) const { return m_Index; }

		[[nodiscard]] auto operator==(const UniqueHandle& other) const
		{
			return m_Index == other.m_Index && m_Container == other.m_Container;
		}

        template<typename U> requires std::is_base_of_v<U, T>
        operator UniqueHandle<t_Container, U>(void) const
        {
            return UniqueHandle<t_Container, U>(m_Container, this->release());
        }

		[[nodiscard]] operator bool(void) const
		{
			return m_Container && m_Index < m_Container->capacity();
		}
	private:
		t_Container* m_Container = nullptr;
		u64 m_Index = k_InvalidHandle;

        template<typename To, typename C, typename From>
        friend UniqueHandle<C, To> dynamic_handle_cast(UniqueHandle<C, From>&&);
	};

    template<typename To, typename t_Container, typename From>
    UniqueHandle<t_Container, To> dynamic_handle_cast(UniqueHandle<t_Container, From>&& handle)
    {
        if (!handle)
            return nullptr;

        using FromPtr = decltype(handle.ptr());
        using ToPtr = std::conditional_t<
            std::is_const_v<std::remove_pointer_t<FromPtr>>,
            const To*,
            To*
        >;

        if (auto casted = dynamic_cast<ToPtr>(handle.ptr()))
            return UniqueHandle<t_Container, To>(handle.m_Container, handle.release());

        return nullptr;
    }

    template<typename t_Container>
    struct HandleControlBlock {
        t_Container* container;
        u64 index;
        std::atomic<u64> strong_count;
        std::atomic<u64> weak_count;

        HandleControlBlock(t_Container* c, u64 i)
        : container(c), index(i), strong_count(1), weak_count(0)
        {}
        ~HandleControlBlock(void) = default;

        void inc_strong_count(void) { this->strong_count.fetch_add(1, std::memory_order_relaxed); }
        void dec_strong_count(void) { this->strong_count.fetch_sub(1, std::memory_order_relaxed); }

        void inc_weak_count(void) { this->weak_count.fetch_add(1, std::memory_order_relaxed); }
        void dec_weak_count(void) { this->weak_count.fetch_sub(1, std::memory_order_relaxed); }
    };

    template<typename t_Container, typename T>
    class WeakHandle;

    template<typename t_Container, typename T>
    class SharedHandle {
    public:
        using ControlBlock = HandleControlBlock<t_Container>;

        SharedHandle(void) = default;
        ~SharedHandle(void) { this->release(); }

        SharedHandle(std::nullptr_t) : m_ControlBlock(nullptr) {}
        SharedHandle& operator=(std::nullptr_t)
        {
            this->release();
            return *this;
        }

        explicit SharedHandle(ControlBlock* cb)
        : m_ControlBlock(cb)
        {
            if (m_ControlBlock)
                m_ControlBlock->inc_strong_count();
        }

        explicit SharedHandle(t_Container* container, u64 index = k_InvalidHandle)
        : m_ControlBlock(nullptr)
        {
            if (container && index < container->capacity())
                m_ControlBlock = new ControlBlock(container, index);
        }

        SharedHandle(const SharedHandle& other)
        : SharedHandle(other.m_ControlBlock)
        {}

        SharedHandle& operator=(const SharedHandle& other)
        {
            if (m_ControlBlock == other.m_ControlBlock)
                return *this;

            this->release();
            if ((m_ControlBlock = other.m_ControlBlock))
                m_ControlBlock->inc_strong_count();

            return *this;
        }

        SharedHandle(SharedHandle&& other) noexcept
        : m_ControlBlock(std::exchange(other.m_ControlBlock, nullptr))
        {}

        SharedHandle& operator=(SharedHandle&& other) noexcept
        {
            if (m_ControlBlock == other.m_ControlBlock)
                return *this;

            this->release();
            m_ControlBlock = std::exchange(other.m_ControlBlock, nullptr);

            return *this;
        }

        void swap(SharedHandle& other)
        {
            std::swap(m_ControlBlock, other.m_ControlBlock);
        }

        void release(void)
        {
            if (!m_ControlBlock)
                return;

            m_ControlBlock->dec_strong_count();
            if (!m_ControlBlock->strong_count.load(std::memory_order_relaxed))
            {
                if (*this)
                    m_ControlBlock->container->remove_at(m_ControlBlock->index);

                if (!m_ControlBlock->weak_count.load(std::memory_order_relaxed))
                    delete m_ControlBlock;
            }
            m_ControlBlock = nullptr;
        }

        [[nodiscard]] T* ptr(void)
        {
            if (!*this)
                return nullptr;

            return &m_ControlBlock->container->operator[](m_ControlBlock->index);
        }

        [[nodiscard]] const T* ptr(void) const
        {
            if (!*this)
                return nullptr;

            return &m_ControlBlock->container->operator[](m_ControlBlock->index);
        }

        [[nodiscard]] inline T& operator*(void) { return *this->ptr(); }
        [[nodiscard]] inline const T& operator*(void) const { return *this->ptr(); }

        [[nodiscard]] inline T* operator->(void) { return this->ptr(); }
        [[nodiscard]] inline const T* operator->(void) const { return this->ptr(); }

        [[nodiscard]] inline u64 index(void) const { return m_ControlBlock ? m_ControlBlock->index : k_InvalidHandle; }
        [[nodiscard]] inline t_Container* container(void) const { return m_ControlBlock ? m_ControlBlock->container : nullptr; }

        [[nodiscard]] inline u64 strong_count(void) const { return m_ControlBlock ? m_ControlBlock->strong_count.load() : 0; }
        [[nodiscard]] inline u64 weak_count(void) const { return m_ControlBlock ? m_ControlBlock->weak_count.load() : 0; }

        template<typename U> requires std::is_base_of_v<U, T>
        operator SharedHandle<t_Container, U>(void) const
        {
            return SharedHandle<t_Container, U>(m_ControlBlock);
        }

        [[nodiscard]] operator bool(void) const
        {
            return m_ControlBlock && m_ControlBlock->container && m_ControlBlock->index < m_ControlBlock->container->capacity();
        }
    private:
        friend class WeakHandle<t_Container, T>;

        ControlBlock* m_ControlBlock = nullptr;

        template<typename To, typename C, typename From>
        friend SharedHandle<C, To> dynamic_handle_cast(const SharedHandle<C, From>&);
    };

    template<typename To, typename t_Container, typename From>
    SharedHandle<t_Container, To> dynamic_handle_cast(const SharedHandle<t_Container, From>& handle)
    {
        if (!handle)
            return nullptr;

        using FromPtr = decltype(handle.ptr());
        using ToPtr = std::conditional_t<
            std::is_const_v<std::remove_pointer_t<FromPtr>>,
            const To*,
            To*
        >;

        if (auto casted = dynamic_cast<ToPtr>(handle.ptr()))
            return SharedHandle<t_Container, To>(handle.m_ControlBlock);

        return nullptr;
    }

    template<typename t_Container, typename T>
    class WeakHandle {
    public:
        using ControlBlock = HandleControlBlock<t_Container>;

        WeakHandle(void) = default;
        ~WeakHandle(void) { this->release(); }

        WeakHandle(std::nullptr_t) : m_ControlBlock(nullptr) {}
        WeakHandle& operator=(std::nullptr_t)
        {
            this->release();
            return *this;
        }

        WeakHandle(const SharedHandle<t_Container, T>& shared)
        : m_ControlBlock(shared.m_ControlBlock)
        {
            if (m_ControlBlock)
                m_ControlBlock->inc_weak_count();
        }

        WeakHandle(const WeakHandle& other)
        : m_ControlBlock(other.m_ControlBlock)
        {
            if (m_ControlBlock)
                m_ControlBlock->inc_weak_count();
        }

        WeakHandle& operator=(const WeakHandle& other)
        {
            if (m_ControlBlock == other.m_ControlBlock)
                return *this;

            this->release();
            m_ControlBlock = other.m_ControlBlock;

            if (m_ControlBlock)
                m_ControlBlock->inc_weak_count();

            return *this;
        }

        WeakHandle(WeakHandle&& other) noexcept
        : m_ControlBlock(std::exchange(other.m_ControlBlock, nullptr))
        {}

        WeakHandle& operator=(WeakHandle&& other) noexcept
        {
            if (m_ControlBlock == other.m_ControlBlock)
                return *this;

            this->release();
            m_ControlBlock = std::exchange(other.m_ControlBlock, nullptr);

            return *this;
        }

        void swap(WeakHandle& other)
        {
            std::swap(m_ControlBlock, other.m_ControlBlock);
        }

        void release(void)
        {
            if (!m_ControlBlock)
                return;

            m_ControlBlock->dec_weak_count();
            if (!m_ControlBlock->weak_count.load(std::memory_order_relaxed))
            {
                if (!m_ControlBlock->strong_count.load(std::memory_order_relaxed))
                    delete m_ControlBlock;
            }
            m_ControlBlock = nullptr;
        }

        [[nodiscard]] SharedHandle<t_Container, T> lock(void) const
        {
            if (this->expired())
                return nullptr;

            return SharedHandle<t_Container, T>(m_ControlBlock);
        }

        [[nodiscard]] bool expired(void) const
        {
            return !m_ControlBlock || !m_ControlBlock->strong_count.load(std::memory_order_relaxed);
        }

        [[nodiscard]] u64 strong_count(void) const { return m_ControlBlock ? m_ControlBlock->strong_count.load() : 0; }
        [[nodiscard]] u64 weak_count(void) const { return m_ControlBlock ? m_ControlBlock->weak_count.load() : 0; }

        template<typename U> requires std::is_base_of_v<U, T>
        operator WeakHandle<t_Container, U>(void) const
        {
            return WeakHandle<t_Container, U>(m_ControlBlock);
        }

        [[nodiscard]] operator bool(void) const { return m_ControlBlock; }
    private:
        ControlBlock* m_ControlBlock = nullptr;

        template<typename To, typename C, typename From>
        friend WeakHandle<C, To> dynamic_handle_cast(const WeakHandle<C, From>&);
    };

    template<typename To, typename t_Container, typename From>
    WeakHandle<t_Container, To> dynamic_handle_cast(const WeakHandle<t_Container, From>& handle)
    {
        if (!handle)
            return nullptr;

        auto locked = handle.lock();

        using FromPtr = decltype(locked.ptr());
        using ToPtr = std::conditional_t<
            std::is_const_v<std::remove_pointer_t<FromPtr>>,
            const To*,
            To*
        >;

        if (auto casted = dynamic_cast<ToPtr>(locked.ptr()))
            return WeakHandle<t_Container, To>(handle.m_ControlBlock);

        return nullptr;
    }
} // namespace Na

#endif // NA_HANDLES_HPP