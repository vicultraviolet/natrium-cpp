#if !defined(NATRIUM_INCLUDE_ALL)
#define NATRIUM_INCLUDE_ALL

#include "./Core.hpp"
#include "./Core/Context.hpp"
#include "./Core/Logger.hpp"
#include "./Core/Event.hpp"
#include "./Core/Window.hpp"
#include "./Core/Input.hpp"
#include "./Core/DeltaTime.hpp"

#include "./Layers/Layer.hpp"
#include "./Layers/LayerManager.hpp"

#include "./Assets/Asset.hpp"
#include "./Assets/AssetRegistry.hpp"
#include "./Assets/ImageAsset.hpp"
#include "./Assets/ShaderAsset.hpp"
#include "./Assets/ModelAsset.hpp"
#include "./Assets/JsonAsset.hpp"
#include "./Assets/RendererSettingsAsset.hpp"

#include "./Graphics/Device.hpp"
#include "./Graphics/Renderer/RendererCore.hpp"
#include "./Graphics/Pipeline.hpp"
#include "./Graphics/Buffers/VertexBuffer.hpp"
#include "./Graphics/Buffers/IndexBuffer.hpp"
#include "./Graphics/Buffers/UniformBuffer.hpp"
#include "./Graphics/Buffers/StorageBuffer.hpp"
#include "./Graphics/Texture.hpp"
#include "./Graphics/Renderer/Renderer.hpp"

#include "./Math/Camera3dData.hpp"

#include "./Core/Application.hpp"

#include "./Layers/ImGuiLayer.hpp"

// entry point
#include "./Main.hpp"

#endif // NATRIUM_INCLUDE_ALL