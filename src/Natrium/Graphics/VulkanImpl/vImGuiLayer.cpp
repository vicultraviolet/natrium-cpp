#include "Pch.hpp"
#include "Natrium/Graphics/VulkanImpl/vImGuiLayer.hpp"

#if !defined(NA_DISABLE_IMGUI)

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Natrium/Graphics/VulkanImpl/vDevice.hpp"

#include "Natrium/Graphics/VulkanImpl/vRenderer.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_vulkan.h>

namespace Na {
	extern ImGuiKey TranslateToImGuiKey(Key key);
} // namespace Na

namespace Na::VulkanImpl {
	ImGuiLayer::ImGuiLayer(View<const Graphics::Renderer> _renderer, i64 priority, bool demo_window_shown)
	: Na::ImGuiLayer(_renderer, priority, demo_window_shown)
	{
		auto renderer = static_ref_cast<const Renderer>(_renderer);

        vk::DescriptorPoolSize pool_size;
        pool_size.type = vk::DescriptorType::eCombinedImageSampler;
        pool_size.descriptorCount = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE;

        vk::DescriptorPoolCreateInfo create_info;
        create_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        create_info.poolSizeCount = 1;
        create_info.pPoolSizes = &pool_size;
        create_info.maxSets = pool_size.descriptorCount;

        m_DescriptorPool = Device::Get()->logical_device().createDescriptorPool(create_info);

        auto msaa = Device::Get()->vk_limits().vk_msaa_sample_count_if(renderer->settings()->multisampling_enabled());

        ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = VK_API_VERSION_1_0; 
        init_info.Instance = Device::Get()->instance();
        init_info.PhysicalDevice = Device::Get()->physical_device();
        init_info.Device = Device::Get()->logical_device();
        init_info.QueueFamily = Device::Get()->graphics_queue_index();
        init_info.Queue = Device::Get()->graphics_queue();
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = m_DescriptorPool;
        init_info.RenderPass = renderer->window_data().render_pass();
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = (u32)renderer->window_data().images().size();
        init_info.MSAASamples = (VkSampleCountFlagBits)msaa;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&init_info);
	}

    ImGuiLayer::~ImGuiLayer(void)
    {
        ImGui_ImplVulkan_Shutdown();

        if (m_DescriptorPool)
        {
            Device::Get()->logical_device().destroyDescriptorPool(m_DescriptorPool);
            m_DescriptorPool = nullptr;
        }
	}

    void ImGuiLayer::begin(void)
    {
        auto renderer = static_ref_cast<const Renderer>(this->renderer());

        ImGuiIO& io = ImGui::GetIO();
        const Window& window = renderer->window_data().window();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (Key k = 0; k < Keys::k_Last; k++)
        {
            bool down = glfwGetKey(window.native(), k);
            io.AddKeyEvent(TranslateToImGuiKey(k), down);
        }

        if (!this->enabled())
            return;

        double mouse_x, mouse_y;
        glfwGetCursorPos(window.native(), &mouse_x, &mouse_y);
        io.AddMousePosEvent((float)mouse_x, (float)mouse_y);

        for (int button = 0; button < 3; button++)
        {
            int state = glfwGetMouseButton(window.native(), button);
            io.AddMouseButtonEvent(button, state == GLFW_PRESS);
        }
    }

    void ImGuiLayer::end(void) const
    {
        auto renderer = static_ref_cast<const Renderer>(this->renderer());

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderer->current_frame().cmd_buffer);
	}
} // namespace Na

#endif // NA_DISABLE_IMGUI
