#include <iostream>
#include <vector>
#include <set>
#ifdef _WIN32
    #include "SDL.h" 
    #include "SDL_vulkan.h"
#elif __linux__
    #include "SDL2/SDL.h" 
    #include "SDL2/SDL_vulkan.h"
#endif
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"
#include "VkBootstrap.h"
#include "glm/glm.hpp"

#define SHS_VULKAN_VERSION_MAJOR 1
#define SHS_VULKAN_VERSION_MINOR 2

int   g_window_width  = 640;
int   g_window_height = 480;
char* g_app_name      = "Hello Vulkan API with SDL2";

int main(int argc, char *argv[]) {


    // Vulkan ачаалахад хэрэглэнэ
    VkInstance       _instance;
    VkSurfaceKHR     _surface;
    VkPhysicalDevice _selected_GPU;
    VkDevice         _device;

    // Рэндэр үр дүнг авахад хэрэгтэй
    VkSwapchainKHR           _swapchain;
    VkFormat                 _swapchain_image_format;
    std::vector<VkImage>     _swapchain_images;
    std::vector<VkImageView> _swapchain_image_views;

    // Төхөөрөмжрүү команд илгээн ажиллуулахад хэрэглэнэ
    VkQueue         _graphics_queue;        // командууд чихэх queue
    uint32_t        _graphics_queue_family; // queue объектийн төрөл
    VkCommandPool   _command_pool;          // команд буфер зохицуулна
    VkCommandBuffer _main_command_buffer;   // командуудыг бичиж хадгалах буфер

    // Рэндэр хийхэд хэрэглэнэ
    VkRenderPass               _render_pass;
    std::vector<VkFramebuffer> _framebuffers;



    // SDL2-г Vulkan дэмжилттэйгээр эхлүүлэх
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *_window = SDL_CreateWindow(
        g_app_name, 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        g_window_width, g_window_height,
        SDL_WINDOW_VULKAN);
    std::cout << "Initialized SDL2 window with Vulkan mode" << std::endl;
    // Vulkan instance үүсгэх
    vkb::InstanceBuilder builder;
    auto inst_ret = builder.set_app_name(g_app_name)
                        //.request_validation_layers()
                        .use_default_debug_messenger()
                        .build ();
    vkb::Instance vkb_inst = inst_ret.value();
    _instance = vkb_inst.instance;
    std::cout << "Created Vulkan instance" << std::endl;
    // SDL-ээр үүсгэсэн цонхны vulkan surface-г авах
    SDL_Vulkan_CreateSurface(_window, _instance, &_surface);
    std::cout << "Connected SDL2 windowing system with Vulkan instance" << std::endl;
    // Физик төхөөрөмжүүдийг илрүүлэх
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    auto phys_ret = selector.set_surface(_surface)
                        .set_minimum_version(SHS_VULKAN_VERSION_MAJOR, SHS_VULKAN_VERSION_MINOR)
                        .select();
    if (!phys_ret) {
        std::cerr << "Failed to select Vulkan Physical Device. Error: " << phys_ret.error().message() << "\n";
        return -1;
    } else {
        std::cout << "Detected physical devices which can be work with Vulkan API" << std::endl;
    }
    _selected_GPU = phys_ret.value().physical_device;
    // Илрүүлсэн төхөөрөмжтэй тулж ажиллах хувьсагчууд
    vkb::DeviceBuilder device_builder{ phys_ret.value() };
    auto dev_ret = device_builder.build();
    vkb::Device vkb_device = dev_ret.value();
    _device       = vkb_device.device;
    std::cout << "Initialized logical device which will work with physical device" << std::endl;



    // Swap Chain үүсгэх
    vkb::SwapchainBuilder swapchainBuilder{_selected_GPU, _device, _surface};
    vkb::Swapchain vkbSwapchain = swapchainBuilder
        .use_default_format_selection()
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(g_window_width, g_window_height)
        .build()
        .value();
    //swapchain болон түүнтэй хамаарарлтай зурагнуудын объектүүдийг хадгалах
    _swapchain              = vkbSwapchain.swapchain;
    _swapchain_images       = vkbSwapchain.get_images().value();
    _swapchain_image_views  = vkbSwapchain.get_image_views().value();
    _swapchain_image_format = vkbSwapchain.image_format;
    std::cout << "Swapchain is created" << std::endl;



    // График командууд илгээх queue объект үүсгэх
    _graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
    _graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
    // График queue рүү команд илгээх бүтэц
    VkCommandPoolCreateInfo command_pool_info = {};
    command_pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.pNext            = nullptr;
    command_pool_info.queueFamilyIndex = _graphics_queue_family;
    command_pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCreateCommandPool(_device, &command_pool_info, nullptr, &_command_pool);
    // Command Pool-ээр командууд бичих бүтэц үүсгэх
    VkCommandBufferAllocateInfo command_buffer_info = {};
    command_buffer_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_info.pNext              = nullptr;
    command_buffer_info.commandPool        = _command_pool;
    command_buffer_info.commandBufferCount = 1;
    command_buffer_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkAllocateCommandBuffers(_device, &command_buffer_info, &_main_command_buffer);
    std::cout << "Graphics queue and Command related objects are created" << std::endl;



    // RenderPass дотор ерөнхийдөө рэндэрлэх үйл ажиллагаа явагдана
    VkAttachmentDescription color_attachment = {};
    color_attachment.format         = _swapchain_image_format;
    color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    VkAttachmentReference color_attachment_ref = {};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount    = 1;
	subpass.pColorAttachments       = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments    = &color_attachment;
	render_pass_info.subpassCount    = 1;
	render_pass_info.pSubpasses      = &subpass;
	vkCreateRenderPass(_device, &render_pass_info, nullptr, &_render_pass);





    // Ашигласан нөөцүүдээ суллах
    vkDestroyCommandPool(_device, _command_pool, nullptr);
    vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    vkDestroyRenderPass(_device, _render_pass, nullptr);
    

    for (int i=0; i<_swapchain_image_views.size(); i++) {
        vkDestroyImageView(_device, _swapchain_image_views[i], nullptr);
    }
    vkDestroyDevice(_device, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);
    SDL_DestroyWindow(_window);
    SDL_Quit();
    return 0;
}
