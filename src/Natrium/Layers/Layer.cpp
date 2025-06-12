#include "Pch.hpp"
#include "Natrium/Layers/Layer.hpp"

namespace Na {
	bool Layer::enabled(void) const
	{
		return (m_State & LayerStateBits::Enabled) != LayerStateBits::None;
	}

	bool Layer::updatable(void) const
	{
		return (m_State & LayerStateBits::Updatable) != LayerStateBits::None;
	}

	bool Layer::visible(void) const
	{
		return (m_State & LayerStateBits::Visible) != LayerStateBits::None;
	}

	void Layer::set_enabled(bool enabled)
	{
		if (enabled)
			m_State |= LayerStateBits::Enabled;
		else
			m_State &= ~LayerStateBits::Enabled;
	}

	void Layer::set_updatable(bool updatable)
	{
		if (updatable)
			m_State |= LayerStateBits::Updatable;
		else
			m_State &= ~LayerStateBits::Updatable;
	}

	void Layer::set_visible(bool visible)
	{
		if (visible)
			m_State |= LayerStateBits::Visible;
		else
			m_State &= ~LayerStateBits::Visible;
	}
} // namespace Na
