#if !defined(NATRIUM_INCLUDE_ALL)
#define NATRIUM_INCLUDE_ALL

#include "./Core.hpp"

#include "./Core/Context.hpp"
#include "./Core/Logger.hpp"
#include "./Core/Event.hpp"
#include "./Core/Window.hpp"
#include "./Core/Input.hpp"
#include "./Core/DeltaTime.hpp"
#include "./Core/UUID.hpp"

#include "./Layers/Layer.hpp"
#include "./Layers/LayerManager.hpp"

#include "./Assets/Asset.hpp"
#include "./Assets/AssetManager.hpp"
#include "./Assets/HostImage.hpp"
#include "./Assets/RendererSettingsAsset.hpp"
#include "./Assets/HostMesh.hpp"
#include "./Assets/TextAsset.hpp"

#include "./Graphics/Device.hpp"
#include "./Graphics/Renderer.hpp"
#include "./Graphics/Pipelines.hpp"
#include "./Graphics/Buffer.hpp"
#include "./Graphics/DeviceImage.hpp"
#include "./Graphics/Sampler.hpp"
#include "./Graphics/VertexAttributes.hpp"

#include "./Math/Camera3dData.hpp"

#include "./Layers/ImGuiLayer.hpp"

#include "./Audio/Audio_Listener.hpp"
#include "./Audio/Audio_Context.hpp"
#include "./Audio/Wav.hpp"
#include "./Audio/Audio_Buffer.hpp"
#include "./Audio/Audio_Source.hpp"

#include "./ECS/Entity.hpp"
#include "./ECS/Component.hpp"
#include "./ECS/ComponentContainer.hpp"
#include "./ECS/ECS_Registry.hpp"
#include "./ECS/ECS_RegistrySerializer.hpp"
#include "./ECS/Scene.hpp"
#include "./ECS/EntityHandle.hpp"

#include "./HL/UniformManager_HL.hpp"
#include "./HL/Pipeline_HL.hpp"
#include "./HL/DeviceMesh_HL.hpp"
#include "./HL/Texture_HL.hpp"
#include "./HL/Application_HL.hpp"

// entry point
#include "./Main.hpp"

#endif // NATRIUM_INCLUDE_ALL