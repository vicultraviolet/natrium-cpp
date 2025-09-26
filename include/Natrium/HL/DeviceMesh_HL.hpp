#if !defined(NA_HL_DEVICE_MESH_HPP)
#define NA_HL_DEVICE_MESH_HPP

#include "Natrium/Assets/Asset.hpp"

#include "Natrium/Assets/HostMesh.hpp"
#include "Natrium/Graphics/Buffer.hpp"

namespace Na::HL {
	class DeviceMesh : public Asset {
	public:
		DeviceMesh(void) = default;

		DeviceMesh(
			const UUID_t& uuid,
			WeakRef<const HostMesh> host_mesh
		);

		DeviceMesh(
			const UUID_t& uuid,
			const Vertex* vertices, u32 vertex_count,
			const u32* indices, u32 index_count
		);

		[[nodiscard]] inline View<const Graphics::Buffer> vertex_buffer(void) const { return m_VertexBuffer; }
		[[nodiscard]] inline View<const Graphics::Buffer> index_buffer(void) const { return m_IndexBuffer; }

		[[nodiscard]] inline u32 vertex_count(void) const { return m_VertexCount; }
		[[nodiscard]] inline u32 index_count(void) const { return m_IndexCount; }

		[[nodiscard]] inline static const Graphics::VertexAttributes& GetVertexAttributes(void) { return HostMesh::GetVertexAttributes(); }

		[[nodiscard]] inline operator bool(void) const override { return m_VertexBuffer && m_IndexBuffer; }
	private:
		UniqueRef<Graphics::Buffer> m_VertexBuffer;
		UniqueRef<Graphics::Buffer> m_IndexBuffer;

		u32 m_VertexCount = 0;
		u32 m_IndexCount = 0;
	};
} // namespace Na::HL

#endif // NA_HL_DEVICE_MESH_HPP