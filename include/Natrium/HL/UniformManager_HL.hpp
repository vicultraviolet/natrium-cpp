#if !defined(NA_HL_UNIFORM_MANAGER_HPP)
#define NA_HL_UNIFORM_MANAGER_HPP

#include "Natrium/HL/UniformSetLayouts_HL.hpp"
#include "Natrium/Graphics/UniformSet.hpp"

namespace Na::HL {
	using UniformSetHandle = u64;

	class UniformManager {
	public:
		UniformManager(void) = default;
		~UniformManager(void) = default;

		Ref<Graphics::UniformSetLayout> init_layout(
			UniformSetIndex layout_index,
			const std::initializer_list<Graphics::UniformBinding>& bindings
		);

		Ref<Graphics::UniformSetLayout> init_layout(
			UniformSetIndex layout_index,
			Ref<Graphics::UniformSetLayout> layout
		);

		UniformSetHandle create_set(
			UniformSetIndex layout_index,
			View<const Graphics::Renderer> renderer
		);

		[[nodiscard]] inline Ref<Graphics::UniformSet> set(UniformSetHandle handle) const { return m_Sets[handle]; }
		[[nodiscard]] inline Ref<Graphics::UniformSetLayout> set_layout(UniformSetIndex index) const { return m_SetLayouts[index]; }

		[[nodiscard]] inline u64 set_count(void) const { return m_Sets.size(); }

		[[nodiscard]] inline const auto& sets(void) const { return m_Sets; }
		[[nodiscard]] inline auto& sets(void) { return m_Sets; }

		[[nodiscard]] inline const auto& set_layouts(void) const { return m_SetLayouts; }
		[[nodiscard]] inline auto& set_layouts(void) { return m_SetLayouts; }
	private:
		UniformSetLayouts m_SetLayouts;
		ArrayList<Ref<Graphics::UniformSet>> m_Sets{ 4 };
	};
} // namespace Na::HL

#endif // NA_HL_UNIFORM_MANAGER_HPP