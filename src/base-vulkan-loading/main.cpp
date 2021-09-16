#include <iostream>
#include <vector>
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


    unsigned int ext_count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &ext_count, nullptr);

    std::vector<const char*> ext_names(ext_count);
    SDL_Vulkan_GetInstanceExtensions(window, &ext_count, ext_names.data());

    std::cout<<"Энэ техник хангамжинд "<<ext_count<<" ширхэг extension байна :"<<std::endl;
    for (unsigned int i = 0; i < ext_count; i++) {
        std::cout<<i<<": "<<ext_names[i]<<std::endl;
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
