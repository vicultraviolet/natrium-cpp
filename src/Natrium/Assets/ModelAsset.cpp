#include "Pch.hpp"
#include "Natrium/Assets/ModelAsset.hpp"

#include <tiny_obj_loader/tiny_obj_loader.h>

namespace Na {
    static void loadObj(const std::filesystem::path& path, Na::ArrayList<Vertex>& vertices, Na::ArrayList<u32>& indices)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str());
        NA_ASSERT(result, "Failed to load {}: {} {}", path.string().c_str(), warn, err);

        u64 index_count = 0;
        for (const auto& shape : shapes)
            index_count += shape.mesh.indices.size();

        indices.reallocate(index_count);

        std::unordered_map<Vertex, u32> unique_vertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{
                    .position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    },
                    .uv_coord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    }
                };

                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = (u32)(vertices.size());
                    vertices.emplace(vertex);
                }

                indices.emplace(unique_vertices[vertex]);
            }
        }
    }

    void ModelAsset::load(const std::filesystem::path& path)
    {
		loadObj(path, m_Vertices, m_Indices);
    }

    static const Graphics::VertexAttributes vertexAttributes = {
        Graphics::VertexAttribute(Graphics::VertexAttributeType::Vec3),
        Graphics::VertexAttribute(Graphics::VertexAttributeType::Vec2)
    };
    const Graphics::VertexAttributes& ModelAsset::VertexAttributes(void)
    {
        return vertexAttributes;
    }
} // namespace Na
