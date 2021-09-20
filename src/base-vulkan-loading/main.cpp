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

#define VK_BOOTSTRAP_WERROR      false
#define SHS_VULKAN_VERSION_MAJOR 1
#define SHS_VULKAN_VERSION_MINOR 2

int   g_window_width  = 640;
int   g_window_height = 480;
char* g_app_name      = "Hello Vulkan API with SDL2";

int main(int argc, char *argv[]) {

    VkInstance   _instance;
    VkSurfaceKHR _surface;
    VkDevice     _device;

    // SDL2-г Vulkan дэмжилттэйгээр эхлүүлэх
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *_window = SDL_CreateWindow(
        g_app_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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


    // Илрүүлсэн төхөөрөмжтэй тулж ажиллах хувьсагчууд
    vkb::DeviceBuilder device_builder{ phys_ret.value() };
    auto dev_ret = device_builder.build();
    vkb::Device vkb_device = dev_ret.value();
    _device = vkb_device.device;
    std::cout << "Initialized logical device which will work with physical device" << std::endl;



    std::cout<<"It looks like everything is fine for now..."<<std::endl;

    // Ашигласан нөөцүүдээ суллах
    vkDestroyDevice(_device, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);
    SDL_DestroyWindow(_window);
    SDL_Quit();
    return 0;
}
