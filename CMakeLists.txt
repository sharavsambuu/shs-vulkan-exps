cmake_minimum_required(VERSION 3.18)

project("shs-vulkan-experiments")

if(WIN32)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/windows-cmakes/")
endif()


include(FetchContent)

# Vulkan Bootstrapping library
FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        v0.4
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)
set(VK_BOOTSTRAP_WERROR OFF)
set(VK_BOOTSTRAP_TEST   OFF)

# Sol2 Lua Cpp binding library
FetchContent_Declare(
    fetch_sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2
    GIT_TAG        v3.2.2
)
FetchContent_MakeAvailable(fetch_sol2)


# My projects
add_subdirectory ("src/base-vulkan-loading")
add_subdirectory ("src/hello-cpp-lua")




