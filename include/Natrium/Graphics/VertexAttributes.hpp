#if !defined(NA_VERTEX_ATTRIBUTES_HPP)
#define NA_VERTEX_ATTRIBUTES_HPP

#include "Natrium/Core.hpp"

namespace Na::Graphics {
	enum class VertexAttributeType : u8 {
		None = 0,
		Float, Vec2, Vec3, Vec4
	};
	u64 SizeOfVertexAttribute(VertexAttributeType type);

	struct VertexAttribute {
		VertexAttributeType type;
		u32 binding;
	};

	class VertexAttributes {
	public:
		VertexAttributes(void) = default;
		VertexAttributes(u32 attribute_count) : m_Attributes(init::init, (u64)attribute_count) {}

		VertexAttributes(const std::initializer_list<VertexAttribute>& attributes) : m_Attributes(attributes) {}

		~VertexAttributes(void) = default;
		void destroy(void);

		void add(u32 location, VertexAttributeType type, u32 binding = 0);
		[[nodiscard]] inline VertexAttribute get(u32 location) const { return m_Attributes[location]; }

		[[nodiscard]] u64 size(void) const;

		[[nodiscard]] inline u32 count(void) const { return (u32)m_Attributes.size(); }
		[[nodiscard]] inline const ArrayList<VertexAttribute>& attributes(void) const { return m_Attributes; }

		[[nodiscard]] inline operator bool(void) const { return !m_Attributes.empty(); }
	private:
		ArrayList<VertexAttribute> m_Attributes; 
	};
} // namespace Na::Graphics

#endif // NA_VERTEX_ATTRIBUTES_HPP