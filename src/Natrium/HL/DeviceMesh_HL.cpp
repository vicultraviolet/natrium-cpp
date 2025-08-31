#include "Pch.hpp"
#include "Natrium/HL/DeviceMesh_HL.hpp"

namespace Na::HL {
	DeviceMesh::DeviceMesh(WeakRef<HostMesh> _host_mesh)
	{
		auto host_mesh = _host_mesh.lock();
		NA_ASSERT(host_mesh, "Failed to create DeviceMesh: Invalid host mesh reference!");

		m_VertexCount = host_mesh->vertex_count();
		m_IndexCount = host_mesh->index_count();

		m_VertexBuffer = Graphics::MakeVertexBuffer(host_mesh->vertex_data_size());
		m_VertexBuffer->set_data(host_mesh->vertices().ptr());

		m_IndexBuffer = Graphics::MakeIndexBuffer(host_mesh->index_count());
		m_IndexBuffer->set_data(host_mesh->indices().ptr());
	}

	DeviceMesh::DeviceMesh(
		const Vertex* vertices, u32 vertex_count,
		const u32* indices, u32 index_count
	)
	: m_VertexCount(vertex_count), m_IndexCount(index_count)
	{
		m_VertexBuffer = Graphics::MakeVertexBuffer((u64)vertex_count * sizeof(Vertex));
		m_VertexBuffer->set_data(vertices);

		m_IndexBuffer = Graphics::MakeIndexBuffer(index_count);
		m_IndexBuffer->set_data(indices);
	}
} // namespace Na::HL
