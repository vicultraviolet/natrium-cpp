#if !defined(NA_MODEL_ASSET_HPP)
#define NA_MODEL_ASSET_HPP

#include "Natrium/Assets/Asset.hpp"
#include "Natrium/Graphics/VertexAttributes.hpp"

namespace Na {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv_coord;

		[[nodiscard]] inline bool operator==(const Vertex& other) const { return position == other.position && uv_coord == other.uv_coord; }
	};

	class ModelAsset : public Asset {
	public:
		ModelAsset(void) = default;
		ModelAsset(const UUID_t& uuid) : Asset(uuid) {}

		~ModelAsset(void) = default;

		void load(const std::filesystem::path& path) override;

		[[nodiscard]] static const Graphics::VertexAttributes& VertexAttributes(void);

		[[nodiscard]] inline u64 vertex_data_size(void) const { return m_Vertices.size() * sizeof(Vertex); }
		[[nodiscard]] inline u64 index_data_size(void) const { return m_Indices.size() * sizeof(u32); }

		[[nodiscard]] inline u32 vertex_count(void) const { return (u32)m_Vertices.size(); }
		[[nodiscard]] inline u32 index_count(void) const { return (u32)m_Indices.size(); }

		[[nodiscard]] inline ArrayList<Vertex>& vertices(void) { return m_Vertices; }
		[[nodiscard]] inline const ArrayList<Vertex>& vertices(void) const { return m_Vertices; }

		[[nodiscard]] inline ArrayList<u32>& indices(void) { return m_Indices; }
		[[nodiscard]] inline const ArrayList<u32>& indices(void) const { return m_Indices; }

		[[nodiscard]] inline operator bool(void) const override { return !m_Vertices.empty() && !m_Indices.empty(); };
	private:
		ArrayList<Vertex> m_Vertices;
		ArrayList<u32> m_Indices;
	};
} // namespace Na

namespace std {
	template<>
	struct hash<Na::Vertex> {
		size_t operator()(const Na::Vertex& vertex) const
		{
			return (
				hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec2>()(vertex.uv_coord) << 1)
			);
		}
	};
} // namespace std

#endif // NA_MODEL_ASSET_HPP