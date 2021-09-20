cmake_minimum_required(VERSION 3.21)

project("shs-vulkan-experiments")

if(WIN32)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/windows-cmakes/")
endif()

add_subdirectory ("src/base-vulkan-loading")




