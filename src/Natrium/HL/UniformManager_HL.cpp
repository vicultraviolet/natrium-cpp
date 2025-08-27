#include "Pch.hpp"
#include "Natrium/HL/UniformManager_HL.hpp"

namespace Na::HL {
	Ref<Graphics::UniformSetLayout> UniformManager::init_layout(
		UniformSetIndex layout_index,
		const std::initializer_list<Graphics::UniformBinding>& bindings
	)
	{
		return m_SetLayouts[layout_index] = Graphics::UniformSetLayout::Make(bindings);
	}

	Ref<Graphics::UniformSetLayout> UniformManager::init_layout(
		UniformSetIndex layout_index,
		Ref<Graphics::UniformSetLayout> layout
	)
	{
		return m_SetLayouts[layout_index] = layout;
	}

	UniformSetHandle UniformManager::create_set(
		UniformSetIndex layout_index,
		View<const Graphics::Renderer> renderer
	)
	{
		return m_Sets.emplace_back(
			Graphics::UniformSet::Make(m_SetLayouts[layout_index], renderer)
		);
	}
} // namespace Na::HL
