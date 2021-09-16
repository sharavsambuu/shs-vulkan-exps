cmake_minimum_required(VERSION 3.7)

project("shs-vulkan-experiments")

if(WIN32)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
  #message(STATUS "cmake module path ${CMAKE_MODULE_PATH}")
endif()

add_subdirectory ("src/base-vulkan-loading")




