#if !defined(NA_PIPELINE_HPP)
#define NA_PIPELINE_HPP

#include "Natrium/Graphics/Device.hpp"
#include "Natrium/Graphics/Renderer/RendererCore.hpp"
#include "Natrium/Assets/ShaderAsset.hpp"

namespace Na {
	using PipelineShaderInfos = std::initializer_list<vk::PipelineShaderStageCreateInfo>;

	enum class ShaderAttributeType : u8 {
		None = 0,
		Float, Vec2, Vec3, Vec4
	};

	struct ShaderAttribute {
		u32 location;
		ShaderAttributeType type;
	};

	enum class ShaderAttributeInputRate : u8 {
		Vertex = 0, Instance
	};

	struct ShaderAttributeBinding {
		u32 binding;
		ShaderAttributeInputRate input_rate;
		std::initializer_list<ShaderAttribute> attributes;
	};

	using ShaderAttributeLayout = std::initializer_list<ShaderAttributeBinding>;

	enum class ShaderUniformType : u8 {
		None = 0,
		UniformBuffer, StorageBuffer, Texture, 

		UBO = UniformBuffer,
		SSBO = StorageBuffer
	};

	struct ShaderUniform {
		u32 binding;
		ShaderUniformType type;
		ShaderStage shader_stage;
	};
	using ShaderUniformLayout = std::initializer_list<ShaderUniform>;

	struct PushConstantSpecs {
		ShaderStage shader_stage;
		u32 size;
		u32 offset = 0;
	};
	using PushConstantLayout = std::initializer_list<PushConstantSpecs>;

	class GraphicsPipeline {
	public:
		GraphicsPipeline(void) = default;
		GraphicsPipeline(
			RendererCore& renderer_core,
			const PipelineShaderInfos& handles = {},
			const ShaderAttributeLayout& vertex_buffer_layout = {},
			const ShaderUniformLayout& uniform_data_layout = {},
			const PushConstantLayout& push_constant_layout = {}
		);
		void destroy(void);
		inline ~GraphicsPipeline(void) { this->destroy(); }

		GraphicsPipeline(const GraphicsPipeline& other) = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline& other) = delete;

		GraphicsPipeline(GraphicsPipeline&& other);
		GraphicsPipeline& operator=(GraphicsPipeline&& other);

		/// 
		/// warning:
		/// uniforms not being bound with the order specified via the constructor is undefined behaviour
		/// 
		template<typename T>
		inline void bind_uniform(u32 binding, const T& uniform) { this->_bind_uniform(binding, &uniform); }

		[[nodiscard]] inline vk::Pipeline pipeline(void) const { return m_Pipeline; }

		[[nodiscard]] inline vk::DescriptorSetLayout descriptor_layout(void) const { return m_DescriptorLayout; }
		[[nodiscard]] inline vk::PipelineLayout layout(void) const { return m_Layout; }

		[[nodiscard]] inline vk::DescriptorPool descriptor_pool(void) const { return m_DescriptorPool; }
		[[nodiscard]] inline const vk::DescriptorSet& descriptor_set(void) const { return m_DescriptorSet; }

		[[nodiscard]] inline ArrayList<u32>& dynamic_offsets(void) { return m_DynamicOffsets; }
		[[nodiscard]] inline const ArrayList<u32>& dynamic_offsets(void) const { return m_DynamicOffsets; }
		[[nodiscard]] inline u32 dynamic_offset_count(void) const { return m_DynamicOffsetCount; }
		[[nodiscard]] inline u32 dynamic_offset_index(void) const { return m_DynamicOffsetIndex; }
		inline void increment_dynamic_offset_index(void) { m_DynamicOffsetIndex++; }

		[[nodiscard]] inline operator bool(void) const { return m_Pipeline; }
	private:
		void _bind_uniform(u32 binding, const void* uniform);
	private:
		vk::Pipeline m_Pipeline;

		vk::DescriptorSetLayout m_DescriptorLayout;
		vk::PipelineLayout m_Layout;

		vk::DescriptorPool m_DescriptorPool;
		vk::DescriptorSet m_DescriptorSet;

		ArrayList<u32> m_DynamicOffsets;
		u32 m_DynamicOffsetCount = 0;
		u32 m_DynamicOffsetIndex = 0;
	};
} // namespace Na

#endif // NA_PIPELINE_HPP