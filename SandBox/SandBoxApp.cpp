#include "SandBox/SandBoxApp.h"

#include <string>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <Engine/Core/Input.h>
#include <Engine/Scene/SceneMngr.h>
#include <Engine/GameObject/Components/MeshCmpt.h>
#include <Engine/Asset/LoadOBJ.h>
#include <Engine/Graphics/Vulkan/Initializers.h>
#include <Engine/Renderer/Renderer.h>



SandBoxApp::SandBoxApp(const std::string& title, const std::string& root, int width, int height)
    : Application(title, root, width, height)
{
    VkExtent2D swapchainExtent = Renderer::Instance().GetSwapCainExtent();
    this->drawExtent = swapchainExtent;

    // 1. create draw image
    vk::ImageBuilder builder;
	builder.SetExtent(VkExtent3D{swapchainExtent.width, swapchainExtent.height, 1})
		.SetUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT |
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.SetFormat(VK_FORMAT_R16G16B16A16_SFLOAT)
		.SetVmaUsage(VMA_MEMORY_USAGE_GPU_ONLY)
		.SetVmaRequiredFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    colorAttachment = builder.Build(Renderer::Instance().GetContext());
    
    // create depth image
	builder.SetUsage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
		.SetFormat(VK_FORMAT_D32_SFLOAT);
    depthAttachment = builder.Build(Renderer::Instance().GetContext());

    // 2. create render passes
    VkRenderingAttachmentInfo colorInfo = vk::init::attachment_info(colorAttachment.imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	VkRenderingAttachmentInfo depthInfo = vk::init::depth_attachment_info(depthAttachment.imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    geometryPass = std::make_unique<GeometryPass>("../Assets/Shaders/Spirv/mesh.vert.spv",
        "../Assets/Shaders/Spirv/mesh.frag.spv", std::vector<VkRenderingAttachmentInfo>{colorInfo}, depthInfo);
    geometryPass->SetResoluton(swapchainExtent);

}

Application* CreateApplication()
{

    auto application = new SandBoxApp("test"," ", 2000, 1200);

    // 1. Create a scene
    auto scene = SceneMngr::Instance().CreateScene("SandBox");
    application->scene = scene;

    // 2. Create gameobjects
    auto mesh = LoadObj("../Assets/Meshes/Stanford_Bunny.obj");

    auto bunny = scene->AddGameObject("bunny");
    auto meshCmpt = bunny->AddComponent<MeshCmpt>();
    meshCmpt->mesh = mesh;
    bunny->transformCmpt->SetPosition(glm::vec3(0.1, 0.1, -1));

    // 3. Add a camera
    auto cam = scene->AddGameObject("MainCamera");
    cam->transformCmpt->SetPosition(glm::vec3(0, 0, 0));
    scene->SetMainCamera(cam->AddComponent<CameraCmpt>());
    application->yaw = 0;
    application->pitch = 0;

    // 4. Prepare for rendering
    application->geometryPass->Prepare(application->scene);
    
    return application;
}

SandBoxApp::~SandBoxApp()
{
    vkDeviceWaitIdle(Renderer::Instance().GetVkDevice());
    vk::Image::DestroyImage(Renderer::Instance().GetContext(), colorAttachment);
    vk::Image::DestroyImage(Renderer::Instance().GetContext(), depthAttachment);
}

// void SandBoxApp::Update()
// {
//     if (Input::IsKeyPressed(K)) {
//         auto bunny = SceneMngr::Instance().get_object("bunny");
//         auto transform = bunny->get_component<Transform>();
//         auto rigid_body = bunny->get_component<RigidBodyDynamic>();
//         transform->set_position(glm::vec3(0.f, 1.f, 0.f));
//         transform->set_rotation(glm::quat(1.f, 0.f, 0.f, 0.f));
//         rigid_body->init_velocity();

//     }
//     if (Input::IsKeyPressed(L)) {
//         auto bunny = SceneMngr::Instance().get_object("bunny");
//         auto transform = bunny->get_component<Transform>();
//         auto rigid_body = bunny->get_component<RigidBodyDynamic>();
//         rigid_body->init_velocity(glm::vec3(0.f, 3.f, -6.f));
//         rigid_body->set_lauched(true);
//     }
//     auto bunny = SceneMngr::Instance().get_object("bunny");
//     auto wall = SceneMngr::Instance().get_object("wall");
//     auto gridbox = SceneMngr::Instance().get_object("gridbox");
//     auto mesh_collider = bunny->get_component<MeshCollider>();
//     auto wall_plane_collider = wall->get_component<PlaneCollider>();
//     auto ground_plane_collider = gridbox->get_component<PlaneCollider>();
//     check_collision(wall_plane_collider, mesh_collider);
//     check_collision(ground_plane_collider, mesh_collider);
// }

void SandBoxApp::Render()
{
    // 1. prepare imgui data (not executing render commands here)
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();
    if (ImGui::Begin("Stats")) {
        ImGui::End();
    }

    ImGui::Render();

    // 2. render frame
    if (auto frame = Renderer::Instance().BeginFrame()) {
        VkCommandBuffer cmd = frame->mainCommandBuffer;
        auto presentImg = frame->presentImage;

        // draw background
        vk::Image::TransitImageLayout(Renderer::Instance().GetContext(), cmd, colorAttachment.vkImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
        Renderer::Instance().DrawBackGround(colorAttachment.imageView, drawExtent);

        // draw geometry
        vk::Image::TransitImageLayout(Renderer::Instance().GetContext(), cmd, colorAttachment.vkImage, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        vk::Image::TransitImageLayout(Renderer::Instance().GetContext(), cmd, depthAttachment.vkImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL_KHR);
        geometryPass->Draw(frame);

        // copy image to present image
        vk::Image::TransitImageLayout(Renderer::Instance().GetContext(), cmd, colorAttachment.vkImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        vk::Image::TransitImageLayout(Renderer::Instance().GetContext(), cmd, presentImg.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        vk::Image::CopyImage(Renderer::Instance().GetContext(), cmd, colorAttachment.vkImage, presentImg.image, drawExtent, drawExtent);
        
        // draw imgui
        vk::Image::TransitImageLayout(Renderer::Instance().GetContext(), cmd, presentImg.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        Renderer::Instance().DrawImgui(presentImg.imageView, drawExtent);
        vk::Image::TransitImageLayout(Renderer::Instance().GetContext(), cmd, presentImg.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        Renderer::Instance().EndFrame();
    }
}