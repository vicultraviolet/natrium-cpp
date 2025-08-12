#if !defined(NA_EXPECTED_HPP)
#define NA_EXPECTED_HPP

#include "Natrium/Core.hpp"

namespace Na {
	template <class T, class E>
	union ExpectedData {
		T value;
		E error;

		ExpectedData(void) {}
		~ExpectedData(void) {}
	};

	template<typename E>
	class Unexpected {
	public:
		using value_type = E;
		using error_type = E;

		Unexpected(const E& error) : m_Error(error) {}
		Unexpected(E&& error) noexcept : m_Error(std::move(error)) {}

		template<typename... t_Args>
		explicit Unexpected(t_Args&&... args) : m_Error(std::forward<t_Args>(args)...) {}

		[[nodiscard]] inline const E& error(void) const { return m_Error; }
		[[nodiscard]] inline E& error(void) { return m_Error; }
	private:
		E m_Error;
	};

	template<typename T, typename E>
	class Expected {
	public:
		using Data = ExpectedData<T, E>;

		using value_type = T;
		using error_type = E;

		Expected(void) = default;
		~Expected(void)
		{
			if (m_HasValue)
				std::destroy_at(&m_Data.value);
			else
				std::destroy_at(&m_Data.error);
		}

		explicit Expected(bool has_value) : m_HasValue(has_value) {}

		Expected(const T& value)
		{
			std::construct_at(&m_Data.value, value);
			m_HasValue = true;
		}

		Expected(T&& value) noexcept
		{
			std::construct_at(&m_Data.value, std::move(value));
			m_HasValue = true;
		}

		Expected(const Unexpected<E>& unexpected)
		{
			std::construct_at(&m_Data.error, unexpected.error());
		}

		Expected(Unexpected<E>&& unexpected) noexcept
		{
			std::construct_at(&m_Data.error, std::move(unexpected.error()));
		}

		Expected(const Expected& other)
		{
			if ((m_HasValue = other.m_HasValue))
				std::construct_at(&m_Data.value, other.m_Data.value);
			else
				std::construct_at(&m_Data.error, other.m_Data.error);
		}

		Expected& operator=(const Expected& other)
		{
			if (this == &other)
				return *this;

			if (m_HasValue)
				std::destroy_at(&m_Data.value);
			else
				std::destroy_at(&m_Data.error);

			if ((m_HasValue = other.m_HasValue))
				std::construct_at(&m_Data.value, other.m_Data.value);
			else
				std::construct_at(&m_Data.error, other.m_Data.error);

			return *this;
		}

		Expected(Expected&& other) noexcept
		{
			if ((m_HasValue = other.m_HasValue))
				std::construct_at(&m_Data.value, std::move(other.m_Data.value));
			else
				std::construct_at(&m_Data.error, std::move(other.m_Data.error));
		}

		Expected& operator=(Expected&& other) noexcept
		{
			if (this == &other)
				return *this;

			if (m_HasValue)
				std::destroy_at(&m_Data.value);
			else
				std::destroy_at(&m_Data.error);

			if ((m_HasValue = other.m_HasValue))
				std::construct_at(&m_Data.value, std::move(other.m_Data.value));
			else
				std::construct_at(&m_Data.error, std::move(other.m_Data.error));

			return *this;
		}

		[[nodiscard]] const T& value(void) const
		{
			NA_ASSERT(m_HasValue, "Failed to access Expected value: no value present!");
			return m_Data.value;
		}

		[[nodiscard]] T& value(void)
		{
			NA_ASSERT(m_HasValue, "Failed to access Expected value: no value present!");
			return m_Data.value;
		}

		[[nodiscard]] const T& value_or(const T& default_value) const
		{
			return m_HasValue ? m_Data.value : default_value;
		}

		[[nodiscard]] T& value_or(T& default_value)
		{
			return m_HasValue ? m_Data.value : default_value;
		}

		[[nodiscard]] const E& error(void) const
		{
			NA_ASSERT(!m_HasValue, "Failed to access Expected error: no error present!");
			return m_Data.error;
		}

		[[nodiscard]] E& error(void)
		{
			NA_ASSERT(!m_HasValue, "Failed to access Expected error: no error present!");
			return m_Data.error;
		}

		template<typename t_Fn> requires std::invocable<t_Fn, T>
		auto and_then(t_Fn&& fn)
		{
			using ReturnType = std::invoke_result_t<t_Fn, T>;
			static_assert(
				IsExpected<ReturnType>::value,
				"Function must return Expected type"
			);

			if (m_HasValue)
			{
				return fn(m_Data.value);
			}

			return this->_propagate_error<ReturnType>();
		}

		template<typename t_Fn> requires std::invocable<t_Fn, T>
		auto transform(t_Fn&& fn)
		{
			using U = std::remove_cvref_t<std::invoke_result_t<t_Fn, T>>;

			if (m_HasValue)
			{
				return Expected<U, E>(fn(m_Data.value));
			}

			return Expected<U, E>(Unexpected(m_Data.error));
		}

		template<typename t_Fn> requires std::invocable<t_Fn, E>
		auto or_else(t_Fn&& fn)
		{
			using Result = std::invoke_result_t<t_Fn, E>;

			static_assert(
				IsExpected<Result>::value,
				"Function must return Expected type"
			);
			static_assert(
				std::is_same<typename Result::value_type, T>::value,
				"Must return Expected with same value type"
			);

			if (!m_HasValue)
			{
				return fn(m_Data.error);
			}

			return *this;
		}

		[[nodiscard]] inline bool has_value(void) const { return m_HasValue; }
		[[nodiscard]] inline bool has_error(void) const { return !m_HasValue; }

		[[nodiscard]] inline operator bool(void) const { return m_HasValue; }
	private:
		template<typename Tested>
		struct IsExpected : std::false_type {};

		template<typename U, typename V>
		struct IsExpected<Expected<U, V>> : std::true_type {};

		template<typename t_Result>
		t_Result _propagate_error(void)
		{
			if constexpr (std::is_same<E, typename t_Result::error_type>::value)
			{
				return t_Result(Unexpected(m_Data.error));
			} else
			{
				return t_Result(Unexpected(typename t_Result::error_type(m_Data.error)));
			}
		}

		Data m_Data;
		bool m_HasValue = false;
	};
} // namespace Na

#endif // NA_EXPECTED_HPP