#if !defined(NA_ECS_COMPONENT_HPP)
#define NA_ECS_COMPONENT_HPP

#include "Natrium/Core.hpp"

namespace Na::ECS {
	class Component {
	public:
		Component(void) = default;
		virtual ~Component(void) = default;

		virtual nlohmann::json serialize(void) const = 0;
		virtual void deserialize(const nlohmann::json& j) = 0;

		[[nodiscard]] static inline std::string_view GetTypeName(void) { return "component"; }
		[[nodiscard]] virtual inline std::string_view type_name(void) const { return "component"; }
	};

	template<typename T>
	concept ComponentConcept = (
		std::derived_from<T, Component> &&
		requires {
			{ T::GetTypeName() } -> std::convertible_to<std::string_view>;
		}
	);

#define NA_DEFINE_COMPONENT_TYPE_NAME(x) \
	[[nodiscard]] static inline std::string_view GetTypeName(void) { return x; } \
	[[nodiscard]] inline std::string_view type_name(void) const override { return x; }
} // namespace Na::ECS

#endif // NA_ECS_COMPONENT_HPP