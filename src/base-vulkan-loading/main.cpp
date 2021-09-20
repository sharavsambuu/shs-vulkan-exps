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

uint32_t g_window_width  = 640;
uint32_t g_window_height = 480;
char*    g_app_name      = "Hello Vulkan API with SDL2";


int main(int argc, char *argv[]) {

    std::cout << "###############################" << std::endl;
    std::cout << "# Hello Base Vulkan with SDL2 #" << std::endl;
    std::cout << "###############################" << std::endl;

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

    // GPU дээр хийгдсэн ажлын үр дүнг CPU дээр хүлээхэд хэрэглэнэ
    VkSemaphore _present_semaphore, _render_semaphore;
    VkFence     _render_fence;

    // Фрэймийн мэдээлэл
    VkExtent2D _window_extent{g_window_width, g_window_height};



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
    vkb::PhysicalDeviceSelector selector{vkb_inst};
    auto phys_ret = selector.set_surface(_surface)
                        .set_minimum_version(SHS_VULKAN_VERSION_MAJOR, SHS_VULKAN_VERSION_MINOR)
                        .select();
    std::cout << "Detected and selected physical device which can be work with Vulkan API" << std::endl;
    _selected_GPU = phys_ret.value().physical_device;
    // Сонгосон GPU-ий талаархи мэдээллийг авах
    VkPhysicalDeviceProperties gpu_info;
    vkGetPhysicalDeviceProperties(_selected_GPU, &gpu_info);
    std::cout << "Selected device name : " << gpu_info.deviceName << std::endl;
    // Илрүүлсэн төхөөрөмжтэй тулж ажиллах хувьсагчууд
    vkb::DeviceBuilder device_builder{phys_ret.value()};
    auto dev_ret = device_builder.build();
    vkb::Device vkb_device = dev_ret.value();
    _device       = vkb_device.device;
    std::cout << "Initialized logical device which will work with physical device" << std::endl;



    // Swap Chain үүсгэх
    vkb::SwapchainBuilder swapchain_builder{_selected_GPU, _device, _surface};
    vkb::Swapchain vkb_swapchain = swapchain_builder
        .use_default_format_selection()
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(g_window_width, g_window_height)
        .build()
        .value();
    //swapchain болон түүнтэй хамаарарлтай зурагнуудын объектүүдийг хадгалах
    _swapchain              = vkb_swapchain.swapchain;
    _swapchain_images       = vkb_swapchain.get_images().value();
    _swapchain_image_views  = vkb_swapchain.get_image_views().value();
    _swapchain_image_format = vkb_swapchain.image_format;
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
    std::cout << "Render Pass is created" << std::endl;


 
    // swapchain-ий зурагнуудад зориулж framebuffer үүсгэн тохируулах
    // ингэснээр render pass-г зурагнуудруу рэндэр хийлгүүлэх боломжтой болно
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext           = nullptr;
    fb_info.renderPass      = _render_pass;
    fb_info.attachmentCount = 1;
    fb_info.width           = g_window_width;
    fb_info.height          = g_window_height;
    fb_info.layers          = 1;
    const uint32_t swapchain_imagecount = _swapchain_images.size();
    _framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);
    for (int i=0; i<swapchain_imagecount; i++) {
        fb_info.pAttachments = &_swapchain_image_views[i];
        vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffers[i]);
    }
    std::cout << "Framebuffer is created" << std::endl;



    // GPU дээр хийгдсэн үр дүнг CPU дээр авч зэрэгцүүлэх объектүүд
    VkFenceCreateInfo fence_create_info = {};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = nullptr;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(_device, &fence_create_info, nullptr, &_render_fence);
    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;
    vkCreateSemaphore(_device, &semaphore_create_info, nullptr, &_present_semaphore);
    vkCreateSemaphore(_device, &semaphore_create_info, nullptr, &_render_semaphore );
    std::cout << "synchronization primitives are created" << std::endl;




    long one_second   = 1000000000;
    long wait_delay   = (long)(one_second/1);
    long frame_number = 0;

    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&e)!=0) {
            if ((e.type==SDL_QUIT) || (e.type==SDL_KEYDOWN && e.key.keysym.sym==SDLK_ESCAPE)) quit = true;
        }

        // GPU хамгийн сүүлийн фрэймээ рэндэрлэж дуустал нь хүлээх
        vkWaitForFences(_device, 1, &_render_fence, true, wait_delay);
        vkResetFences(_device, 1, &_render_fence);

        // командууд ажиллаж дууссан тул комманд бичилтийг дахин эхлүүлэх
        vkResetCommandBuffer(_main_command_buffer, 0);

        // swapchain-аас зураг авах 
        uint32_t swapchain_image_index;
        vkAcquireNextImageKHR(_device, _swapchain, wait_delay, _present_semaphore, nullptr, &swapchain_image_index);

        VkCommandBuffer cmd = _main_command_buffer;

        // command buffer бичлэгийг эхлүүлэх
        VkCommandBufferBeginInfo cmd_begin_info = {};
        cmd_begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_begin_info.pNext            = nullptr;
        cmd_begin_info.pInheritanceInfo = nullptr;
        cmd_begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &cmd_begin_info);

        // Фрэймийн дугаараас өнгө үүсгэх
        VkClearValue clear_value;
        float flash       = abs(sin(frame_number / 120.f));
        clear_value.color = {{0.0f, flash, flash, 1.0f}};

        // үндсэн render pass эхлүүлэх 
        // swapchain-аас авсан framebuffer дээрхи өнгийг шинэчилнэ
        VkRenderPassBeginInfo rp_info = {};
        rp_info.sType               = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_info.pNext               = nullptr;
        rp_info.renderPass          = _render_pass;
        rp_info.renderArea.offset.x = 0;
        rp_info.renderArea.offset.y = 0;
        rp_info.renderArea.extent   = _window_extent;
        rp_info.clearValueCount     = 1;
        rp_info.pClearValues        = nullptr;
        rp_info.framebuffer         = _framebuffers[swapchain_image_index];
        rp_info.clearValueCount     = 1;
        rp_info.pClearValues        = &clear_value;
        vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

        // ####################################
        // #                                  #
        // # рэндэрлэх командууд энд хийгдэнэ #
        // #                                  #
        // ####################################

        // render pass дуусгах
        vkCmdEndRenderPass(cmd);
        
        // цаашид командууд нэмэхгүй тул command buffer-г өндөрлүүлэх
        vkEndCommandBuffer(cmd);


        // queue-рүү команд илгээхэд бэлтгэх
        // swapchain бэлэн болох үед _present_semaphore нь мэдэгдэнэ
        // рэндэрлэх үйл ажиллагаа дууссан бол _render_semaphore нь мэдэгдэнэ
        VkSubmitInfo submit = {};
        submit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pNext                = nullptr;
        submit.waitSemaphoreCount   = 0;
        submit.pWaitSemaphores      = nullptr;
        submit.pWaitDstStageMask    = nullptr;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &cmd;
        submit.signalSemaphoreCount = 0;
        submit.pSignalSemaphores    = nullptr;
        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.pWaitDstStageMask    = &waitStage;
        submit.waitSemaphoreCount   = 1;
        submit.pWaitSemaphores      = &_present_semaphore;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores    = &_render_semaphore;

        // command buffer-г ажиллуулахаар queue-рүү илгээх
        // график комманд ажиллаж дуустал _render_fence дээр түрдээ блок хийгдэнэ
        vkQueueSubmit(_graphics_queue, 1, &submit, _render_fence);

        // _render_semaphore дээр хүлээгээд гарсан үр дүнг авах
        VkPresentInfoKHR present_info   = {};
        present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext              = nullptr;
        present_info.swapchainCount     = 0;
        present_info.pSwapchains        = nullptr;
        present_info.pWaitSemaphores    = nullptr;
        present_info.waitSemaphoreCount = 0;
        present_info.pImageIndices      = nullptr;
        present_info.pSwapchains        = &_swapchain;
        present_info.swapchainCount     = 1;
        present_info.pWaitSemaphores    = &_render_semaphore;
        present_info.waitSemaphoreCount = 1;
        present_info.pImageIndices      = &swapchain_image_index;
        vkQueuePresentKHR(_graphics_queue, &present_info);

        frame_number++;

    }



    // Ашигласан нөөцүүдээ суллах
    vkDeviceWaitIdle(_device);
    vkDestroyFence(_device, _render_fence, nullptr);
    vkDestroySemaphore(_device, _render_semaphore, nullptr);
    vkDestroySemaphore(_device, _present_semaphore, nullptr);
    vkDestroyCommandPool(_device, _command_pool, nullptr);
    vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    vkDestroyRenderPass(_device, _render_pass, nullptr);
    for (int i=0; i<_framebuffers.size(); i++) {
        vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
    }
    for (int i=0; i<_swapchain_image_views.size(); i++) {
        vkDestroyImageView(_device, _swapchain_image_views[i], nullptr);
    }
    vkDestroyDevice(_device, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);
    SDL_DestroyWindow(_window);
    SDL_Quit();
    std::cout << "Resources are properly deallocated, BYE" << std::endl;


    return 0;
}
