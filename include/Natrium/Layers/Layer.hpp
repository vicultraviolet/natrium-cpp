#if !defined(NA_LAYER_HPP)
#define NA_LAYER_HPP

#include "Natrium/Core.hpp"
#include "Natrium/Core/Event.hpp"

namespace Na {
	enum class LayerStateBits : u8 {
		None = 0,

		Enabled   = NA_BIT(0),
		Updatable = NA_BIT(1),
		Visible   = NA_BIT(2),

		All = Enabled | Updatable | Visible
	};

	class Layer {
	public:
		Layer(i64 priority = 0) : m_Priority(priority) {}
		virtual ~Layer(void) = default;

		virtual void on_attach(void) {}
		virtual void on_detach(void) {}

		virtual void on_event(Event& e) {}
		virtual void update(double dt) {}
		virtual void draw(void) {}
		virtual void imgui_draw(void) {}

		[[nodiscard]] bool enabled(void) const;
		[[nodiscard]] bool updatable(void) const;
		[[nodiscard]] bool visible(void) const;

		virtual void set_enabled(bool enabled);
		virtual void set_updatable(bool updatable);
		virtual void set_visible(bool visible);

		[[nodiscard]] LayerStateBits state(void) const { return m_State; }
		void set_state(LayerStateBits state) { m_State = state; }

		[[nodiscard]] inline i64 priority(void) const { return m_Priority; }
	private:
		friend class LayerManager;

		i64 m_Priority;
		LayerStateBits m_State = LayerStateBits::All;
	};

	template<typename T>
	concept DerivedLayer = std::is_base_of<Layer, T>::value && !std::is_same<Layer, T>::value;

	template<DerivedLayer T, typename... t_Args>
	inline Ref<T> MakeLayer(t_Args&&... __args) { return MakeRef<T>(std::forward<t_Args>(__args)...); }

	inline LayerStateBits operator|(LayerStateBits lhs, LayerStateBits rhs) { return (LayerStateBits)((u8)lhs | (u8)rhs); }
	inline LayerStateBits operator&(LayerStateBits lhs, LayerStateBits rhs) { return (LayerStateBits)((u8)lhs & (u8)rhs); }
	inline LayerStateBits operator^(LayerStateBits lhs, LayerStateBits rhs) { return (LayerStateBits)((u8)lhs ^ (u8)rhs); }
	inline LayerStateBits operator~(LayerStateBits state) { return (LayerStateBits)(~(u8)state); }

	inline LayerStateBits& operator|=(LayerStateBits& lhs, LayerStateBits rhs) { lhs = lhs | rhs; return lhs; }
	inline LayerStateBits& operator&=(LayerStateBits& lhs, LayerStateBits rhs) { lhs = lhs & rhs; return lhs; }
	inline LayerStateBits& operator^=(LayerStateBits& lhs, LayerStateBits rhs) { lhs = lhs ^ rhs; return lhs; }

	inline bool operator==(LayerStateBits lhs, LayerStateBits rhs) { return (u8)lhs == (u8)rhs; }
	inline bool operator!=(LayerStateBits lhs, LayerStateBits rhs) { return (u8)lhs != (u8)rhs; }
} // namespace Na

#endif // NA_LAYER_HPP