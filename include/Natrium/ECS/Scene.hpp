#if !defined(NA_ECS_SCENE_HPP)
#define NA_ECS_SCENE_HPP

#include "Natrium/Assets/Asset.hpp"
#include "Natrium/ECS/ECS_Registry.hpp"
#include "Natrium/ECS/ECS_RegistrySerializer.hpp"

namespace Na::ECS {
	class Scene : public Asset {
	public:
		[[nodiscard]] nlohmann::json serialize(const RegistrySerializer& reg_serializer) const;
		void deserialize(const RegistrySerializer& reg_serializer, const nlohmann::json& j);

		[[nodiscard]] inline Registry& registry(void) { return m_Registry; }
		[[nodiscard]] inline const Registry& registry(void) const { return m_Registry; }

		[[nodiscard]] inline operator bool(void) const override { return !m_UUID.is_nil(); }
	private:
		Registry m_Registry;
	};
} // namespace Na::ECS

#endif // NA_ECS_SCENE_HPP