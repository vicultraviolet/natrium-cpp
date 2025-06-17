#include "Pch.hpp"
#include "Natrium/Assets/ModelAsset.hpp"

#include <tiny_obj_loader/tiny_obj_loader.h>

#if defined(NA_PLATFORM_WINDOWS)
#define C_STR string().c_str
#elif defined(NA_PLATFORM_LINUX)
#define C_STR c_str
#else
#define C_STR c_str
#endif

namespace Na {
    static void loadObj(const std::filesystem::path& path, Na::ArrayList<Vertex>& vertices, Na::ArrayList<u32>& indices)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.C_STR());
        NA_ASSERT(result, "Failed to load {}: {} {}", path.C_STR(), warn, err);

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

	AssetHandle<ModelAsset> ModelAsset::Load(const std::filesystem::path& path)
	{
        AssetHandle<ModelAsset> asset = Ref<ModelAsset>::Make();

        if (path.extension() == ".obj")
            loadObj(path, asset->m_Vertices, asset->m_Indices);
        else
            throw std::runtime_error(NA_FORMAT("{} is an unknown or unsupported 3d model file format!", path.extension().C_STR()));

        return asset;
	}

    static ShaderAttributeLayout shader_layout{
        Na::ShaderAttributeBinding{
            .binding = 0,
            .input_rate = Na::AttributeInputRate::Vertex,
            .attributes = {
                Na::ShaderAttribute{
                    .location = 0,
                    .type = Na::ShaderAttributeType::Vec3
                },
                Na::ShaderAttribute{
                    .location = 1,
                    .type = Na::ShaderAttributeType::Vec2
                }
            }
        }
    };
    const ShaderAttributeLayout& ModelAsset::ShaderLayout(void) 
    {
        return shader_layout;
	}

} // namespace Na
