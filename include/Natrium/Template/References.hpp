#if !defined(NA_REF_HPP)
#define NA_REF_HPP

#include "Natrium/Core.hpp"

namespace Na {
	template<typename T>
	class UniqueRef {
	public:
		UniqueRef(void) : m_Ptr(nullptr) {}
		~UniqueRef(void) { this->destroy(); }

		UniqueRef(nullptr_t) : m_Ptr(nullptr) {}
		UniqueRef& operator=(nullptr_t)
		{
			this->destroy();
			return *this;
		}

		UniqueRef(T* ptr) : m_Ptr(ptr) {}
		UniqueRef& operator=(T* ptr)
		{
			this->destroy();
			m_Ptr = ptr;
			return *this;
		}

		template<typename... t_Args>
		static UniqueRef Make(t_Args&&... __args)
		{
			return UniqueRef(new T(std::forward<t_Args>(__args)...));
		}

		UniqueRef(UniqueRef&& other)
		: m_Ptr(std::exchange(other.m_Ptr, nullptr))
		{}

		UniqueRef& operator=(UniqueRef&& other)
		{
			this->destroy();
			m_Ptr = std::exchange(other.m_Ptr, nullptr);
			return *this;
		}

		void swap(UniqueRef& other)
		{
			std::swap(m_Ptr, other.m_Ptr);
		}

		void destroy(void)
		{
			if (!m_Ptr)
				return;

			delete m_Ptr;
			m_Ptr = nullptr;
		}

		T* release(void)
		{
			if (!m_Ptr)
				return nullptr;

			T* temp = m_Ptr;
			m_Ptr = nullptr;
			return temp;
		}

		template<typename U, std::enable_if_t<std::is_base_of_v<U, T>, int> = 0>
		[[nodiscard]] inline operator UniqueRef<U>(void)&& { return UniqueRef<U>(this->release()); }

		[[nodiscard]] inline T* ptr(void) { return m_Ptr; }
		[[nodiscard]] inline const T* ptr(void) const { return m_Ptr; }

		[[nodiscard]] inline T& operator*(void) { return *m_Ptr; }
		[[nodiscard]] inline const T& operator*(void) const { return *m_Ptr; }

		[[nodiscard]] inline T* operator->(void) { return m_Ptr; }
		[[nodiscard]] inline const T* operator->(void) const { return m_Ptr; }

		[[nodiscard]] inline auto operator<=>(const UniqueRef& other) const { return m_Ptr <=> other.m_Ptr; }
		[[nodiscard]] inline auto operator==(const UniqueRef& other) const { return m_Ptr == other.m_Ptr; }

		[[nodiscard]] inline operator bool(void) const { return m_Ptr; }
	private:
		template<typename To, typename From>
		friend UniqueRef<To> static_pointer_cast(UniqueRef<From>&& from);

		template<typename To, typename From>
		friend UniqueRef<To> dynamic_pointer_cast(UniqueRef<From>&& from);

		T* m_Ptr = nullptr;
	};

	template<typename To, typename From>
	UniqueRef<To> static_pointer_cast(UniqueRef<From>&& from)
	{
		using FromPtr = decltype(from.ptr());
		To* casted = (To*)const_cast<std::remove_const_t<std::remove_pointer_t<FromPtr>>*>(from.ptr());
		from.release();
		return UniqueRef<To>(casted);
	}

	template<typename To, typename From>
	UniqueRef<To> dynamic_pointer_cast(UniqueRef<From>&& from)
	{
		using FromPtr = decltype(from.ptr());
		To* casted = dynamic_cast<To*>(const_cast<std::remove_const_t<std::remove_pointer_t<FromPtr>>*>(from.ptr()));
		if (!casted)
			return nullptr;
		from.release();
		return UniqueRef<To>(casted);
	}

	template<typename T>
	struct RefControlBlock {
		T* ptr;
		std::atomic<u64> strong_count;
		std::atomic<u64> weak_count;

		template<typename... t_Args>
		RefControlBlock(t_Args&&... __args)
		: ptr(new T(std::forward<t_Args>(__args)...)), strong_count(0), weak_count(0)
		{}
		~RefControlBlock(void) { delete ptr; }

		void inc_strong_count(void) { this->strong_count.fetch_add(1, std::memory_order_relaxed); }
		void dec_strong_count(void) { this->strong_count.fetch_sub(1, std::memory_order_relaxed); }

		void inc_weak_count(void) { this->weak_count.fetch_add(1, std::memory_order_relaxed); }
		void dec_weak_count(void) { this->weak_count.fetch_sub(1, std::memory_order_relaxed); }
	};

	template<typename T>
	class WeakRef;

	template<typename T>
	class Ref {
	public:
		using ControlBlock = RefControlBlock<T>;

		Ref(void) : m_ControlBlock(nullptr) {}
		~Ref(void) { this->release(); }

		Ref(nullptr_t) : m_ControlBlock(nullptr) {}
		Ref& operator=(nullptr_t)
		{
			this->release();
			return *this;
		}

		explicit Ref(ControlBlock* cb) : m_ControlBlock(cb)
		{
			if (m_ControlBlock)
				m_ControlBlock->inc_strong_count();
		}

		template<typename... t_Args>
		static Ref Make(t_Args&&... __args)
		{
			return Ref(new ControlBlock(std::forward<t_Args>(__args)...));
		}

		Ref(const Ref& other)
		: Ref(other.m_ControlBlock)
		{}

		Ref& operator=(const Ref& other)
		{
			if (m_ControlBlock == other.m_ControlBlock)
				return *this;

			this->release();
			if (other)
			{
				m_ControlBlock = other.m_ControlBlock;
				m_ControlBlock->inc_strong_count();
			}

			return *this;
		}

		Ref(Ref&& other)
		: m_ControlBlock(std::exchange(other.m_ControlBlock, nullptr))
		{}

		Ref& operator=(Ref&& other)
		{
			if (m_ControlBlock == other.m_ControlBlock)
				return *this;

			this->release();
			if (other)
				m_ControlBlock = std::exchange(other.m_ControlBlock, nullptr);
			return *this;
		}

		void swap(Ref& other)
		{
			std::swap(m_ControlBlock, other.m_ControlBlock);
		}

		void release(void)
		{
			if (this->expired())
				return;

			m_ControlBlock->dec_strong_count();
			if (!m_ControlBlock->strong_count)
			{
				delete m_ControlBlock->ptr;
				m_ControlBlock->ptr = nullptr;

				if (!m_ControlBlock->weak_count)
					delete m_ControlBlock;
			}
			m_ControlBlock = nullptr;
		}

		template<typename U, std::enable_if_t<std::is_base_of_v<U, T>, int> = 0>
		[[nodiscard]] operator Ref<U>(void) const
		{
			return Ref<U>((typename Ref<U>::ControlBlock*)this->m_ControlBlock);
		}

		[[nodiscard]] inline u64 strong_count(void) const { return m_ControlBlock->strong_count.load(); }
		[[nodiscard]] inline u64 weak_count(void) const { return m_ControlBlock->weak_count.load(); }

		[[nodiscard]] inline T* ptr(void) { return m_ControlBlock->ptr; }
		[[nodiscard]] inline const T* ptr(void) const { return m_ControlBlock->ptr; }

		[[nodiscard]] inline T& operator*(void) { return *m_ControlBlock->ptr; }
		[[nodiscard]] inline const T& operator*(void) const { return *m_ControlBlock->ptr; }

		[[nodiscard]] inline T* operator->(void) { return m_ControlBlock->ptr; }
		[[nodiscard]] inline const T* operator->(void) const { return m_ControlBlock->ptr; }

		[[nodiscard]] inline auto operator<=>(const Ref& other) const { return m_ControlBlock->ptr <=> other.m_ControlBlock->ptr; }
		[[nodiscard]] inline auto operator==(const Ref& other) const { return m_ControlBlock->ptr == other.m_ControlBlock->ptr; }

		[[nodiscard]] inline bool expired(void) const { return !m_ControlBlock || !m_ControlBlock->strong_count.load(); }
		[[nodiscard]] inline operator bool(void) const { return m_ControlBlock; }
	private:
		friend class WeakRef<T>;

		template<typename To, typename From>
		friend Ref<To> static_pointer_cast(const Ref<From>& from);

		template<typename To, typename From>
		friend Ref<To> dynamic_pointer_cast(const Ref<From>& from);

		ControlBlock* m_ControlBlock;
	};

	template<typename To, typename From>
	Ref<To> static_pointer_cast(const Ref<From>& from)
	{
		if (!from)
			return nullptr;

		return Ref<To>((RefControlBlock<To>*)from.m_ControlBlock);
	}

	template<typename To, typename From>
	Ref<To> dynamic_pointer_cast(const Ref<From>& from)
	{
		if (!from)
			return nullptr;

		using FromPtr = decltype(from.ptr());

		To* casted_ptr = dynamic_cast<To*>(const_cast<std::remove_const_t<std::remove_pointer_t<FromPtr>>*>(from.ptr()));
		if (!casted_ptr)
			return nullptr;
		return Ref<To>((RefControlBlock<To>*)from.m_ControlBlock);
	}

	template<typename T>
	class WeakRef {
	public:
		using ControlBlock = RefControlBlock<T>;

		WeakRef(void) : m_ControlBlock(nullptr) {}
		~WeakRef(void) { this->release(); }

		WeakRef(nullptr_t) : m_ControlBlock(nullptr) {}
		WeakRef& operator=(nullptr_t)
		{
			this->release();
			return *this;
		}

		explicit WeakRef(ControlBlock* cb) : m_ControlBlock(cb)
		{
			if (m_ControlBlock)
				m_ControlBlock->inc_weak_count();
		}

		WeakRef(const WeakRef& other)
		: WeakRef(other.m_ControlBlock)
		{}

		WeakRef& operator=(const WeakRef& other)
		{
			if (m_ControlBlock == other.m_ControlBlock)
				return *this;

			this->release();
			if (other)
			{
				m_ControlBlock = other.m_ControlBlock;
				m_ControlBlock->inc_weak_count();
			}
			return *this;
		}

		WeakRef(WeakRef&& other)
		: m_ControlBlock(std::exchange(other.m_ControlBlock, nullptr))
		{}

		WeakRef& operator=(WeakRef&& other)
		{
			if (m_ControlBlock == other.m_ControlBlock)
				return *this;

			this->release();
			if (other)
				m_ControlBlock = std::exchange(other.m_ControlBlock, nullptr);

			return *this;
		}

		WeakRef(const Ref<T>& ref)
		: WeakRef(ref.m_ControlBlock)
		{}

		WeakRef& operator=(const Ref<T>& ref)
		{
			if (m_ControlBlock == ref.m_ControlBlock)
				return *this;

			this->release();
			if ((m_ControlBlock = ref.m_ControlBlock))
				m_ControlBlock->inc_weak_count();

			return *this;
		}

		void release(void)
		{
			if (this->expired())
				return;

			m_ControlBlock->dec_weak_count();
			if (!m_ControlBlock->weak_count && !m_ControlBlock->strong_count)
				delete m_ControlBlock;

			m_ControlBlock = nullptr;
		}

		[[nodiscard]] Ref<T> lock(void) const
		{
			return m_ControlBlock->strong_count.load() ? Ref<T>(m_ControlBlock) : nullptr;
		}

		template<typename U, std::enable_if_t<std::is_base_of_v<U, T>, int> = 0>
		[[nodiscard]] operator WeakRef<U>(void) const
		{
			return WeakRef<U>((typename WeakRef<U>::ControlBlock*)this->m_ControlBlock);
		}

		[[nodiscard]] inline bool expired(void) const { return !m_ControlBlock || !m_ControlBlock->strong_count.load(); }

		[[nodiscard]] inline u64 strong_count(void) const { return m_ControlBlock->strong_count.load(); }
		[[nodiscard]] inline u64 weak_count(void) const { return m_ControlBlock->weak_count.load(); }

		[[nodiscard]] inline operator bool(void) const { return m_ControlBlock;  }
	private:
		template<typename To, typename From>
		friend WeakRef<To> static_pointer_cast(const WeakRef<From>& from);

		template<typename To, typename From>
		friend WeakRef<To> dynamic_pointer_cast(const WeakRef<From>& from);

		ControlBlock* m_ControlBlock;
	};

	template<typename To, typename From>
	WeakRef<To> static_pointer_cast(const WeakRef<From>& from)
	{
		if (!from)
			return nullptr;

		return WeakRef<To>((RefControlBlock<To>*)from.m_ControlBlock);
	}

	template<typename To, typename From>
	WeakRef<To> dynamic_pointer_cast(const WeakRef<From>& from)
	{
		if (!from)
			return nullptr;

		Ref<From> locked = from.lock();
		if (!locked)
			return nullptr;

		using FromPtr = decltype(locked.ptr());

		To* casted_ptr = dynamic_cast<To*>(const_cast<std::remove_const_t<std::remove_pointer_t<FromPtr>>*>(locked.ptr()));
		if (!casted_ptr)
			return nullptr;

		return WeakRef<To>((RefControlBlock<To>*)from.m_ControlBlock);
	}
} // namespace Na

#endif // NA_REF_HPP