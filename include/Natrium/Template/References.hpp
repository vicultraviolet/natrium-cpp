#if !defined(NA_REF_HPP)
#define NA_REF_HPP

#include "Natrium/Core.hpp"

namespace Na {
	template<typename T>
	class UniqueRef {
	public:
		UniqueRef(void) = default;
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
		UniqueRef(t_Args&&... __args)
		: m_Ptr(tmalloc<T>())
		{
			try
			{
				new(m_Ptr) T(std::forward<t_Args>(__args)...);
			} catch (const std::exception& e)
			{
				free(m_Ptr);
				throw e;
			}
		}

		UniqueRef(UniqueRef&& other)
		: m_Ptr(std::exchange(other.m_Ptr, nullptr))
		{}

		UniqueRef& operator=(UniqueRef&& other)
		{
			this->destroy();

			if (other)
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

			m_Ptr->~T();
			free(m_Ptr);

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
		T* m_Ptr = nullptr;
	};

	template<typename T>
	struct RefControlBlock {
		T* ptr;
		std::atomic<u64> strong_count;
		std::atomic<u64> weak_count;

		template<typename... t_Args>
		RefControlBlock(t_Args&&... __args)
		: ptr(new T(std::forward<t_Args>(__args)...)), strong_count(1), weak_count(0)
		{}
		~RefControlBlock(void) { delete ptr; }
	};

	template<typename T>
	class WeakRef;

	template<typename T>
	class Ref {
	public:
		using ControlBlock = RefControlBlock<T>;

		Ref(void) = default;
		~Ref(void) { this->release(); }

		Ref(nullptr_t) : m_ControlBlock(nullptr) {}
		Ref& operator=(nullptr_t)
		{
			this->release();
			return *this;
		}

		explicit Ref(ControlBlock* cb) : m_ControlBlock(cb) {}

		template<typename... t_Args>
		static Ref Make(t_Args&&... __args)
		{
			return Ref(new ControlBlock(std::forward<t_Args>(__args)...));
		}

		Ref(const Ref& other)
		: m_ControlBlock(other.m_ControlBlock)
		{
			if (m_ControlBlock)
				m_ControlBlock->strong_count++;
		}

		Ref& operator=(const Ref& other)
		{
			if (m_ControlBlock == other.m_ControlBlock)
				return *this;

			this->release();
			if (other)
			{
				m_ControlBlock = other.m_ControlBlock;
				m_ControlBlock->strong_count++;
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
			if (!m_ControlBlock)
				return;

			m_ControlBlock->strong_count--;
			if (!m_ControlBlock->strong_count)
			{
				delete m_ControlBlock->ptr;
				m_ControlBlock->ptr = nullptr;

				if (!m_ControlBlock->weak_count)
					delete m_ControlBlock;
			}
			m_ControlBlock = nullptr;
		}

		[[nodiscard]] inline T* ptr(void) { return m_ControlBlock->ptr; }
		[[nodiscard]] inline const T* ptr(void) const { return m_ControlBlock->ptr; }

		[[nodiscard]] inline std::atomic<u64> strong_count(void) const { return m_ControlBlock->strong_count; }
		[[nodiscard]] inline std::atomic<u64> weak_count(void) const { return m_ControlBlock->weak_count; }

		[[nodiscard]] inline T& operator*(void) { return *m_ControlBlock->ptr; }
		[[nodiscard]] inline const T& operator*(void) const { return *m_ControlBlock->ptr; }

		[[nodiscard]] inline T* operator->(void) { return m_ControlBlock->ptr; }
		[[nodiscard]] inline const T* operator->(void) const { return m_ControlBlock->ptr; }

		[[nodiscard]] inline auto operator<=>(const Ref& other) const { return m_ControlBlock->ptr <=> other.m_ControlBlock->ptr; }
		[[nodiscard]] inline auto operator==(const Ref& other) const { return m_ControlBlock->ptr == other.m_ControlBlock->ptr; }

		[[nodiscard]] inline operator bool(void) const { return m_ControlBlock; }
	private:
		friend class WeakRef<T>;

		ControlBlock* m_ControlBlock;
	};

	template<typename T>
	class WeakRef {
	public:
		using ControlBlock = RefControlBlock<T>;

		WeakRef(void) = default;
		~WeakRef(void) { this->release(); }

		WeakRef(nullptr_t) : m_ControlBlock(nullptr) {}
		WeakRef& operator=(nullptr_t)
		{
			this->release();
			return *this;
		}

		WeakRef(const WeakRef& other)
		: m_ControlBlock(other.m_ControlBlock)
		{
			if (m_ControlBlock)
				m_ControlBlock->weak_count++;
		}

		WeakRef& operator=(const WeakRef& other)
		{
			if (m_ControlBlock == other.m_ControlBlock)
				return *this;

			this->release();
			if (other)
			{
				m_ControlBlock = other.m_ControlBlock;
				m_ControlBlock->weak_count++;
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
		: m_ControlBlock(ref.m_ControlBlock)
		{
			if (m_ControlBlock)
				m_ControlBlock->weak_count++;
		}

		WeakRef& operator=(const Ref<T>& ref)
		{
			if (m_ControlBlock == ref.m_ControlBlock)
				return *this;

			this->release();

			if (m_ControlBlock = ref.m_ControlBlock)
				m_ControlBlock->weak_count++;

			return *this;
		}

		void release(void)
		{
			if (!m_ControlBlock)
				return;

			m_ControlBlock->weak_count--;
			if (!m_ControlBlock->weak_count && !m_ControlBlock->strong_count)
				delete m_ControlBlock;

			m_ControlBlock = nullptr;
		}

		[[nodiscard]] inline bool expired(void) const { return !m_ControlBlock || !m_ControlBlock->strong_count; }

		Ref<T> lock(void) const
		{
			if (this->expired())
				return nullptr;
			
			m_ControlBlock->strong_count++;
			return Ref<T>(m_ControlBlock);
		}

		[[nodiscard]] inline operator bool(void) const { return m_ControlBlock;  }
	private:
		ControlBlock* m_ControlBlock;
	};

	template<typename To, typename From>
	Ref<To> dynamic_pointer_cast(const Ref<From>& from)
	{
		if (!from)
			return nullptr;

		To* casted_ptr = dynamic_cast<To*>(from.ptr());
		if (!casted_ptr)
			return nullptr;

		return Ref<To>((RefControlBlock<To>*)from.m_ControlBlock);
	}

	template<typename To, typename From>
	WeakRef<To> dynamic_pointer_cast(const WeakRef<From>& from)
	{
		if (!from)
			return nullptr;

		Ref<From> locked = from.lock();
		if (!locked)
			return nullptr;

		To* casted_ptr = dynamic_cast<To*>(locked.ptr());
		if (!casted_ptr)
			return nullptr;

		return WeakRef<To>((RefControlBlock<To>*)from.m_ControlBlock);
	}
} // namespace Na

#endif // NA_REF_HPP