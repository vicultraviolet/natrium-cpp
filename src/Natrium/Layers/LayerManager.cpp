#include "Pch.hpp"
#include "Natrium/Layers/LayerManager.hpp"

namespace Na {
	void LayerManager::resort(void)
	{
		for (u64 i = 0; i < m_Layers.size() - 1; i++)
		{
			for (u64 j = 0; j < m_Layers.size() - i - 1; j++)
			{
				if (m_Layers[j]->priority() < m_Layers[j + 1]->priority())
				{
					std::swap(m_Layers[j], m_Layers[j + 1]);
				}
			}
		}
	}

	void LayerManager::attach_layer(Ref<Layer> layer)
	{
		layer->on_attach();

		m_Layers.emplace(layer);
		this->resort();
	}

	void LayerManager::detach_layer(Ref<Layer> layer)
	{
		layer->on_detach();

		if (m_Layers.size() == 1)
		{
			m_Layers.pop();
			return;
		}

		u64 i;
		for (i = 0; i < m_Layers.size(); i++)
			if (m_Layers[i] == layer)
				break;

		NA_ASSERT(i < m_Layers.size(), "Failed to find layer in LayerManager!");

		if (i == m_Layers.size() - 1)
		{
			m_Layers.pop();
			return;
		}

		m_Layers.resize(m_Layers.size() - 1);
		for (; i < m_Layers.size(); i++)
			m_Layers[i] = std::move(m_Layers[i + 1]);
	}

	void LayerManager::detach_all(void)
	{
		for (u64 i = 0; i < m_Layers.size(); i++)
		{
			m_Layers[i]->on_detach();
			m_Layers[i].~Ref();
		}
		m_Layers.resize(0);
	}

	void LayerManager::set_layer_priority(Ref<Layer> layer, i64 new_priority)
	{
		layer->m_Priority = new_priority;
		this->resort();
	}

} // namespace Na
