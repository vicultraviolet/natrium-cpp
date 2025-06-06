#if !defined(NA_LAYER_HPP)
#define NA_LAYER_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Core/Event.hpp"

namespace Na {
	class Layer {
	public:
		Layer(i64 priority = 0) : m_Priority(priority) {}
		virtual ~Layer(void) = default;

		virtual void on_attach(void) {}
		virtual void on_detach(void) {}

		virtual void on_event(Event& e) {}
		virtual void update(double dt) {}
		virtual void draw(void) {}

		[[nodiscard]] inline i64 priority(void) const { return m_Priority; }
	private:
		friend class LayerManager;

		i64 m_Priority;
	};

	template<typename t_Layer = Layer>
	using LayerHandle = Ref<t_Layer>;

	template<typename t_Layer = Layer>
	using WeakLayerHandle = WeakRef<t_Layer>;

	template<typename T>
	concept DerivedLayer = std::is_base_of<Layer, T>::value && !std::is_same<Layer, T>::value;

	template<DerivedLayer T, typename... t_Args>
	inline LayerHandle<T> CreateLayer(t_Args&&... __args) { return Ref<T>::Make(std::forward<t_Args>(__args)...); }
} // namespace Na

#endif // NA_LAYER_HPP