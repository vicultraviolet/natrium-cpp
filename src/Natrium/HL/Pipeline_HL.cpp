#include "Pch.hpp"
#include "Natrium/HL/Pipeline_HL.hpp"

namespace Na::HL {
	Pipeline::Pipeline(const TrianglePipelineCreateInfo& info)
	{
		std::array<
			View<const Graphics::UniformSetLayout>,
			UniformSetIndices::k_Count
		> uniform_set_layouts = {
			(*info.uniform_set_layouts)[0],
			(*info.uniform_set_layouts)[1],
			(*info.uniform_set_layouts)[2],
			(*info.uniform_set_layouts)[3]
		};

		m_Pipeline = Graphics::TrianglePipeline::Make(
			info.render_target,
			*info.vertex_attributes,
			uniform_set_layouts.data(), info.uniform_set_layouts->size(),
			info.shaders.begin(), info.shaders.size()
		);
	}

	Pipeline::Pipeline(const ComputePipelineCreateInfo& info)
	{
		std::array<
			View<const Graphics::UniformSetLayout>,
			UniformSetIndices::k_Count
		> uniform_set_layouts = {
			(*info.uniform_set_layouts)[0],
			(*info.uniform_set_layouts)[1],
			(*info.uniform_set_layouts)[2],
			(*info.uniform_set_layouts)[3]
		};

		m_Pipeline = Graphics::ComputePipeline::Make(
			info.shader,
			uniform_set_layouts.data(), info.uniform_set_layouts->size()
		);
	}

	Pipeline::Pipeline(Pipeline&& other) noexcept
	: m_Pipeline(std::move(other.m_Pipeline))
	{}

	Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
	{
		if (m_Pipeline == other.m_Pipeline)
			return *this;

		m_Pipeline = std::move(other.m_Pipeline);

		return *this;
	}
} // namespace Na::HL
