# Steps

    sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools libglm-dev libsdl2-dev
    mkdir build && cd build && cmake .. && make
    cd ./src/base-vulkan-loading && ./BaseProject

    On windows set environment variable 
        SDL2_PATH="D:\Libs\SDL2-devel-2.0.16-VC\SDL2-2.0.16"
        GLM_PATH="D:\Libs\glm-0.9.9.8\glm"
        etc


# Libraries

    - http://libsdl.org/download-2.0.php
    - https://vulkan.lunarg.com/
    - https://github.com/g-truc/glm
    - https://github.com/charles-lunarg/vk-bootstrap


# References

    - https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php
    - https://github.com/google/shaderc/blob/main/downloads.md
    - https://vulkan-tutorial.com/
    - https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/


