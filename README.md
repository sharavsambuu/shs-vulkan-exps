
# On Ubuntu

    sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools libglm-dev libsdl2-dev liblua5.4-dev



# On windows set environment variable 

    SDL2_PATH="D:\Libs\SDL2-devel-2.0.16-VC\SDL2-2.0.16"
    GLM_PATH="D:\Libs\glm-0.9.9.8\glm"
    LUA54_PATH="D:\Libs\Lua54"
        Download VS2019 static version from
            https://sourceforge.net/projects/luabinaries/files/5.4.2/Windows%20Libraries/Static/
        Sub folders
            - D:\Libs\Lua54\lua-5.4.2_Win32_vc16_lib
            - D:\Libs\Lua54\lua-5.4.2_Win64_vc16_lib
        etc



# Build steps

    mkdir build && cd build && cmake .. && make
    cd ./src/base-vulkan-loading && ./BaseProject



# Libraries

    - http://libsdl.org/download-2.0.php
    - https://vulkan.lunarg.com/
    - https://github.com/g-truc/glm
    - https://github.com/charles-lunarg/vk-bootstrap
    - https://www.lua.org/download.html
    - https://github.com/ThePhD/sol2



# References

    - https://lazyfoo.net/tutorials/SDL/01_hello_SDL/windows/msvc2019/index.php
    - https://github.com/google/shaderc/blob/main/downloads.md
    - https://vulkan-tutorial.com/
    - https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/


