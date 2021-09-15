#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

int main() {
    std::cout<<"Hello Vulkan API\n";

    if (SDL_Init(SDL_INIT_EVERYTHING)==0) {
        std::cout<<"SDL эхлүүлж чадсангүй \n";
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Hello Vulka API"     , 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        600                   ,  // Цонхны өргөн
        400                   ,  // Цонхны өндөр
        SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN
        );

    // vulkan extensions, native window-тэй холбоход хэрэгтэй
    unsigned int ext_count = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(window, &ext_count, nullptr))
    {
        std::cout<<"Vulkan extention ачаалж чадсангүй \n";
        return false;
    }
    std::cout<<"Нийт "<<ext_count<<" ширхэг vulkan extension байна \n";


    SDL_Quit();

    return 0;
}
