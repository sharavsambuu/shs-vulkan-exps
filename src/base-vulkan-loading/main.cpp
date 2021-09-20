#include <iostream>
#include <vector>
#include <set>
#ifdef _WIN32
    #include "SDL.h" 
    #include "SDL_vulkan.h"
    #include "glm/glm.hpp"
#elif __linux__
    #include "SDL2/SDL.h" 
    #include "SDL2/SDL_vulkan.h"
    #include "glm/glm.hpp"
#endif
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
            SDL_WINDOW_VULKAN
            );
    
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
