#include <iostream>
#include <vector>
#include <set>
#include "SDL2/SDL.h" 
#include "SDL2/SDL_vulkan.h"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"


int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow(
            "Hello Vulkan API with SDL2",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
            );


    // Vulkan extension ачаалах, SDL2 цонхтой интерфэйс үүсгэхэд хэрэгтэй.
    unsigned int ext_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &ext_count, nullptr);
    std::vector<const char*> ext_names(ext_count);
    SDL_Vulkan_GetInstanceExtensions(window, &ext_count, ext_names.data());
    std::cout<<"#### "<<ext_count<<" ширхэг vulkan extension байна :"<<std::endl;
    std::vector<std::string> found_extensions;
    for (unsigned int i=0; i<ext_count; i++) {
        std::cout<<i<<": "<<ext_names[i]<<std::endl;
        found_extensions.emplace_back(ext_names[i]);
    }
    found_extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);


    // Vulkan layer extension ачаалах
    std::vector<std::string> found_layers;
    unsigned int instance_layer_count = 0;
    vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
    std::vector<VkLayerProperties> instance_layer_names(instance_layer_count);
    vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layer_names.data());

    std::cout<<"#### "<<instance_layer_count<<" ширхэг instance layer байна :\n";
    std::vector<const char*> valid_instance_layer_names;

    std::set<std::string> lookup_layers;
    lookup_layers.emplace("VK_LAYER_NV_optimus");
    lookup_layers.emplace("VK_LAYER_KHRONOS_validation");
    int count(0);
    found_layers.clear();
    for (const auto& name : instance_layer_names)
    {
        std::cout<<count<<" : "<<name.layerName<<" : "<<name.description<<std::endl;
        auto it = lookup_layers.find(std::string(name.layerName));
        if (it != lookup_layers.end())
            found_layers.emplace_back(name.layerName);
        count++;
    }



    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
