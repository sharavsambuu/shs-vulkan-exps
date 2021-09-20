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
#include "glm/glm.hpp"


int g_window_width  = 640;
int g_window_height = 480;


int main(int argc, char *argv[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
            "Hello Vulkan API with SDL2",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            g_window_width         ,
            g_window_height        ,
            SDL_WINDOW_VULKAN
            );
    


    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
