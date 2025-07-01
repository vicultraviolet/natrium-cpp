#include "Pch.hpp"
#include "Natrium/Graphics/VertexAttributes.hpp"

namespace Na::Graphics {
	u64 SizeOfVertexAttribute(VertexAttributeType type)
	{
		switch (type)
		{
		case VertexAttributeType::Float: return sizeof(float);
		case VertexAttributeType::Vec2:  return sizeof(glm::vec2);
		case VertexAttributeType::Vec3:  return sizeof(glm::vec3);
		case VertexAttributeType::Vec4:  return sizeof(glm::vec4);
		default: return 0;
		}
	}

	void VertexAttributes::destroy(void)
	{
		m_Attributes.destroy();
	}

	void VertexAttributes::add(u32 location, VertexAttributeType type, u32 binding)
	{
		m_Attributes[location].type = type;
		m_Attributes[location].binding = binding;
	}

	u64 VertexAttributes::size(void) const
	{
		u64 size = 0;
		for (const VertexAttribute& attribute : m_Attributes)
		{
			size += SizeOfVertexAttribute(attribute.type);
		}
		return size;
	}
} // namespace Na::Graphics
