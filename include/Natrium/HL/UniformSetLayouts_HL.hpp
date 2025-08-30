#if !defined(NA_HL_UNIFORM_SET_LAYOUTS_HPP)
#define NA_HL_UNIFORM_SET_LAYOUTS_HPP

#include "Natrium/Graphics/UniformSetLayout.hpp"

namespace Na::HL {
	using UniformSetIndex = u8;
	namespace UniformSetIndices {
		constexpr UniformSetIndex k_Global = 0;
		constexpr UniformSetIndex k_Material = 1;
		constexpr UniformSetIndex k_Object = 2;
		constexpr UniformSetIndex k_Misc = 3;

		constexpr UniformSetIndex k_Last = k_Misc;
		constexpr UniformSetIndex k_Count = k_Last + 1;
	} // namespace UniformSetIndices

	struct UniformSetLayouts {
		Ref<Graphics::UniformSetLayout> global;
		Ref<Graphics::UniformSetLayout> material;
		Ref<Graphics::UniformSetLayout> object;
		Ref<Graphics::UniformSetLayout> misc;

		[[nodiscard]] inline auto ptr(void) { return (Ref<Graphics::UniformSetLayout>*)this; }
		[[nodiscard]] inline auto ptr(void) const { return (const Ref<Graphics::UniformSetLayout> *)this; }

		[[nodiscard]] inline Ref<Graphics::UniformSetLayout>& operator[](UniformSetIndex index) { return this->ptr()[index]; }
		[[nodiscard]] inline const Ref<Graphics::UniformSetLayout>& operator[](UniformSetIndex index) const { return this->ptr()[index]; }

		[[nodiscard]] inline u64 size(void) const
		{
			u64 size = 0;

			for (u64 i = 0; i < this->capacity(); i++)
				if (this->ptr()[i])
					size++;

			return size;
		}
		[[nodiscard]] inline u64 capacity(void) const { return UniformSetIndices::k_Count; }
	};
} // namespace Na::HL

#endif // NA_HL_UNIFORM_MANAGER_HPP