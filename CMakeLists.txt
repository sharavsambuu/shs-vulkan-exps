cmake_minimum_required(VERSION 3.18)

project("shs-vulkan-experiments")

set(CMAKE_CXX_STANDARD 20)


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

# pybind11
FetchContent_Declare(
    fetch_pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG        v2.7.1
)
FetchContent_MakeAvailable(fetch_pybind11)

# Sol2 Lua Cpp binding library
FetchContent_Declare(
    fetch_sol2
    GIT_REPOSITORY https://github.com/ThePhD/sol2
    GIT_TAG        v3.2.2
)
FetchContent_MakeAvailable(fetch_sol2)

# Flecs ECS
FetchContent_Declare(
    fetch_flecs
    GIT_REPOSITORY https://github.com/SanderMertens/flecs
    GIT_TAG        v2.4.7
)
FetchContent_MakeAvailable(fetch_flecs)



# My projects
add_subdirectory ("src/base-vulkan-loading")
add_subdirectory ("src/hello-cpp-python")
add_subdirectory ("src/hello-cpp-lua")
add_subdirectory ("src/hello-ecs-lua")




