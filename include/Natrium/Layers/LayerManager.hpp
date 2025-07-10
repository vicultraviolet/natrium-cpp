#if !defined(NA_LAYER_MANAGER_HPP)
#define NA_LAYER_MANAGER_HPP

#include "Natrium/Layers/Layer.hpp"
#include "Natrium/Template/ArrayList.hpp"

namespace Na {
	class LayerManager {
	public:
		using iterator = ArrayList<Ref<Layer>>::iterator;
		using reverse_iterator = ArrayList<Ref<Layer>>::reverse_iterator;
		using const_iterator = ArrayList<Ref<Layer>>::const_iterator;
		using const_reverse_iterator = ArrayList<Ref<Layer>>::const_reverse_iterator;
	public:
		LayerManager(u64 capacity = 2) : m_Layers(capacity) {}
		~LayerManager(void) = default;
		inline void destroy(void) { m_Layers.~ArrayList(); }

		inline void reserve(u64 extra_capacity) { m_Layers.reallocate(m_Layers.capacity() + extra_capacity); }

		void attach_layer(Ref<Layer> layer);
		void detach_layer(Ref<Layer> layer);

		void detach_all(void);

		void set_layer_priority(Ref<Layer> layer, i64 new_priority);

		void resort(void);

		[[nodiscard]] inline bool empty(void) const { return m_Layers.empty(); }
		[[nodiscard]] inline u64 size(void) const { return m_Layers.size(); }

		[[nodiscard]] inline iterator begin(void) { return m_Layers.begin(); }
		[[nodiscard]] inline const_iterator begin(void) const { return m_Layers.begin(); }
		[[nodiscard]] inline const_iterator cbegin(void) const { return m_Layers.cbegin(); }

		[[nodiscard]] inline iterator end(void) { return m_Layers.end(); }
		[[nodiscard]] inline const_iterator end(void) const { return m_Layers.end(); }
		[[nodiscard]] inline const_iterator cend(void) const { return m_Layers.cend(); }

		[[nodiscard]] inline reverse_iterator rbegin(void) { return m_Layers.rbegin(); }
		[[nodiscard]] inline const_reverse_iterator rbegin(void) const { return m_Layers.rbegin(); }
		[[nodiscard]] inline const_reverse_iterator crbegin(void) const { return m_Layers.crbegin(); }

		[[nodiscard]] inline reverse_iterator rend(void) { return m_Layers.rend(); }
		[[nodiscard]] inline const_reverse_iterator rend(void) const { return m_Layers.rend(); }
		[[nodiscard]] inline const_reverse_iterator crend(void) const { return m_Layers.crend(); }
	private:
		ArrayList<Ref<Layer>> m_Layers;
	};
} // namespace Na

#endif // NA_LAYER_MANAGER_HPP