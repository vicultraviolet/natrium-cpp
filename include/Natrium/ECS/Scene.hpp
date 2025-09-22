#if !defined(NA_ECS_SCENE_HPP)
#define NA_ECS_SCENE_HPP

#include "Natrium/Assets/SerializedJson.hpp"
#include "Natrium/ECS/ECS_Registry.hpp"
#include "Natrium/ECS/ECS_RegistrySerializer.hpp"

namespace Na::ECS {
	class Scene : public SerializedJsonAsset {
	public:
		Scene(void) = default;
		Scene(const UUID_t& uuid) : SerializedJsonAsset(uuid) {}

		[[nodiscard]] nlohmann::json serialize(void) const override;
		void deserialize(const nlohmann::json& j) override;

		[[nodiscard]] inline Registry& registry(void) { return m_Registry; }
		[[nodiscard]] inline const Registry& registry(void) const { return m_Registry; }

		[[nodiscard]] inline operator bool(void) const override { return !m_UUID.is_nil(); }
	private:
		Registry m_Registry;
	};
} // namespace Na::ECS

namespace fmt {
	template<>
	struct formatter<Na::ECS::Scene> {
		constexpr auto parse(fmt::format_parse_context& context)
		{
			return context.begin();
		}

		template<typename FormatContext>
		auto format(const Na::ECS::Scene& scene, FormatContext& context) const
		{
			return fmt::format_to(
				context.out(),
				"{}",
				scene.serialize().dump(4)
			);
		}
	};
} // namespace fmt

#endif // NA_ECS_SCENE_HPP