cmake_minimum_required(VERSION 3.18)

project("shs-vulkan-experiments")

if(WIN32)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/windows-cmakes/")
endif()

# Vulkan Bootstrapping library
include(FetchContent)
FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        v0.4
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)
set(VK_BOOTSTRAP_WERROR OFF)
set(VK_BOOTSTRAP_TEST   OFF)


add_subdirectory ("src/base-vulkan-loading")




